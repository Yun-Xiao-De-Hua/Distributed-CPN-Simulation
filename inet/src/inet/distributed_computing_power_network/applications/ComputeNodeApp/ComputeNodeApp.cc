#include<cstring>
#include "ComputeNodeApp.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/HopLimitTag_m.h"
#include "inet/distributed_computing_power_network/message/CpnPathHeader_m.h"


Define_Module(inet::ComputeNodeApp);

namespace inet {

static const int OPTIONAL_CPN_PATH_HEADER_PEEK_FLAGS = Chunk::PF_ALLOW_NULLPTR | Chunk::PF_ALLOW_EMPTY | Chunk::PF_ALLOW_INCOMPLETE;
static const int OPTIONAL_CPN_PATH_HEADER_TYPED_FLAGS = Chunk::PF_ALLOW_NULLPTR | Chunk::PF_ALLOW_INCOMPLETE | Chunk::PF_ALLOW_REINTERPRETATION;

ComputeNodeApp::ComputeNodeApp() {
}

ComputeNodeApp::~ComputeNodeApp() {
    cancelAndDelete(taskCompletionEvent);
}

void ComputeNodeApp::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
       this->computeNodeId = par("computeNodeId");
       this->computeGatewayId = par("computeGatewayId");
       this->localPort = par("localPort");
       this->userNodePort = par("userNodePort");
       this->localAddress = L3AddressResolver().resolve(par("localAddress"));
       this->computeGatewayAddress = L3AddressResolver().resolve(par("computeGatewayAddress"));
       this->computeGatewayPort = par("computeGatewayPort");
       this->computingType = par("computingType");
       this->computingCapacity = par("computingCapacity");
       this->storageCapacity = par("storageCapacity");
       this->maxNetworkBandwidth = par("maxNetworkBandwidth");
       this->computeCost = par("computeCost");
       // availableStorage 表示当前可用容量，由任务入队执行和完成释放动态维护。
       this->availableStorage = this->storageCapacity;
       this->multicastAddress = L3AddressResolver().resolve(par("multicastAddress"));
       this->taskCompletionEvent = new cMessage("TaskExecutionCompleteSelfMsg");
    }

    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        socket.setOutputGate(gate("socketOut"));
        socket.bind(localAddress, localPort);
        socket.setCallback(this);   // 将当前应用实例注册为 socket 的回调处理对象
        socket.setMulticastLoop(false);

        // 加入算力组，等待网关轮询并接收 TASK_DATA 任务。
        socket.joinMulticastGroup(multicastAddress);

        EV_INFO << "ComputeNode" << computeNodeId << " initialized and joined multicast group: " << multicastAddress << endl;
    }

}

// 发送组成员报告
void ComputeNodeApp::sendCgmpReport(simtime_t querySendTime)
{
    EV_INFO << "Start sending CGMP_Report...\n";

    auto payload = makeShared<CgmpReportMsg>();
    payload->setComputeNodeId(computeNodeId);
    payload->setComputeNodePort(localPort);
    payload->setComputingType(computingType);
    payload->setComputeNodeAddress(localAddress);
    payload->setComputingCapacity(computingCapacity);
    payload->setAvailableStorage(availableStorage);
    payload->setMaxNetworkBandwidth(maxNetworkBandwidth);
    payload->setComputeCost(computeCost);
    payload->setQuerySendTime(querySendTime);
    payload->setSendTime(simTime());

    std::string messageType = payload->getMsgType();
    Packet *pkt = new Packet(messageType.c_str());
    pkt->insertAtBack(payload);

    // CGMP_Report 只应在本地接入域内传播，不应穿越上游算力路由器，
    // 因此显式限制 HopLimit 为 1。
    auto hopLimitReq = pkt->addTagIfAbsent<HopLimitReq>();
    hopLimitReq->setHopLimit(1);

    socket.sendTo(pkt, multicastAddress, computeGatewayPort);

    EV_INFO << "ComputeNode" << computeNodeId << " has sent CGMP_Report (TTL=1) to computeGateway" << computeGatewayId
            << ": address=" << localAddress
            << ", port=" << localPort
            << ", computingCapacity=" << computingCapacity << " FLOPs/s"
            << ", computeCost=" << computeCost << " CNY/s"
            << ", availableStorage=" << availableStorage << " MB"
            << ", maxNetworkBandwidth=" << maxNetworkBandwidth << " Mbps"
            << ", querySendTime=" << querySendTime
            << ", sendTime=" << simTime() << "\n";
}


void ComputeNodeApp::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        if (msg == taskCompletionEvent) {
            finishActiveTask();
        }
        else {
            EV_WARN << "ComputeNode" << computeNodeId << " received unknown self message: " << msg->getName() << endl;
            delete msg;
        }
    }
    else {
        // 处理底层传上来的 UDP 数据
        socket.processMessage(msg);
    }
}


// UdpSocket::ICallback
void ComputeNodeApp::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    if (std::strcmp(packet->getName(), "CGMP_Query") == 0) {
        EV_INFO << "Received CGMP_Query. To send CGMP_Report...\n";
        const auto& queryInfo = packet->popAtFront<CgmpQueryMsg>();
        if (queryInfo == nullptr) {
            EV_WARN << "ComputeNode" << computeNodeId << " received CGMP_Query without CgmpQueryMsg payload." << endl;
            delete packet;
            return;
        }
        sendCgmpReport(queryInfo->getSendTime());
        delete packet;
    }
    else if (std::strcmp(packet->getName(), "TASK_DATA") == 0 || std::strcmp(packet->getName(), "TaskDataMsg") == 0) {
        processTaskData(packet);
    }
    else {
        EV_WARN << "ComputeNode" << computeNodeId << " received unknown packet type: " << packet->getName() << endl;
        delete packet;
    }
}

void ComputeNodeApp::processTaskData(Packet *packet)
{
    stripCpnPathHeader(packet);

    const auto& taskData = packet->popAtFront<TaskDataMsg>();
    if (taskData == nullptr) {
        EV_WARN << "ComputeNode" << computeNodeId << " received TASK_DATA without TaskDataMsg payload." << endl;
        delete packet;
        return;
    }

    enqueueTask(taskData);
    delete packet;
    tryStartNextTask();
}

void ComputeNodeApp::enqueueTask(const Ptr<const TaskDataMsg>& taskData)
{
    QueuedTask task;
    task.userId = taskData->getUserId();
    task.taskId = taskData->getTaskId();
    task.userNodeAddress = taskData->getUserNodeAddress();
    task.generationTime = taskData->getGenerationTime();
    task.computingType = taskData->getComputingType();
    task.requiredStorage = taskData->getRequiredStorage();
    task.computingAmount = taskData->getComputingAmount();
    task.transferAmount = taskData->getTransferAmount();
    task.totalDelayRequirement = taskData->getTotalDelayRequirement();
    task.budget = taskData->getBudget();
    task.userMaxBandwidth = taskData->getUserMaxBandwidth();

    taskQueue.push(task);
    EV_INFO << "ComputeNode" << computeNodeId << " enqueued task (" << task.userId << "," << task.taskId
            << "), queueLength=" << taskQueue.size() << endl;
}

void ComputeNodeApp::tryStartNextTask()
{
    if (busy)
        return;

    while (!taskQueue.empty()) {
        QueuedTask task = taskQueue.front();
        taskQueue.pop();

        int failureCode = TASK_OK;
        std::string failureReason;
        if (!validateTask(task, failureCode, failureReason)) {
            EV_WARN << "ComputeNode" << computeNodeId << " rejected task (" << task.userId << "," << task.taskId
                    << "): " << failureReason << endl;
            sendTaskCompletion(task, false, failureCode, failureReason, 0);
            continue;
        }

        startTaskExecution(task);
        return;
    }
}

bool ComputeNodeApp::validateTask(const QueuedTask& task, int& failureCode, std::string& failureReason) const
{
    if (task.computingType != computingType) {
        failureCode = TASK_INVALID_COMPUTING_TYPE;
        failureReason = "requested computing type is not supported by this node";
        return false;
    }
    if (computingCapacity <= 0 || task.computingAmount <= 0 || task.requiredStorage < 0 || task.transferAmount < 0 || task.userMaxBandwidth < 0 || task.totalDelayRequirement < SIMTIME_ZERO) {
        failureCode = TASK_INVALID_RESOURCE_REQUEST;
        failureReason = "task resource request contains invalid values";
        return false;
    }
    if (task.requiredStorage > availableStorage) {
        failureCode = TASK_INSUFFICIENT_STORAGE;
        failureReason = "available storage is insufficient";
        return false;
    }
    if (task.userMaxBandwidth > maxNetworkBandwidth) {
        failureCode = TASK_INSUFFICIENT_BANDWIDTH;
        failureReason = "requested transmission bandwidth exceeds node network bandwidth";
        return false;
    }
    return true;
}

void ComputeNodeApp::startTaskExecution(const QueuedTask& task)
{
    busy = true;
    activeTask = task;
    activeTask.executionStartTime = simTime();
    availableStorage -= task.requiredStorage;

    simtime_t executionTime = SimTime(task.computingAmount / computingCapacity);
    scheduleAt(simTime() + executionTime, taskCompletionEvent);

    EV_INFO << "ComputeNode" << computeNodeId << " started task (" << task.userId << "," << task.taskId
            << "), executionTime=" << executionTime
            << ", availableStorage=" << availableStorage << " MB" << endl;
}

void ComputeNodeApp::finishActiveTask()
{
    if (!busy) {
        EV_WARN << "ComputeNode" << computeNodeId << " received task completion event while idle." << endl;
        return;
    }

    QueuedTask completedTask = activeTask;
    double executionTime = (simTime() - completedTask.executionStartTime).dbl();
    availableStorage += completedTask.requiredStorage;
    if (availableStorage > storageCapacity)
        availableStorage = storageCapacity;

    EV_INFO << "ComputeNode" << computeNodeId << " completed task (" << completedTask.userId << "," << completedTask.taskId
            << "), executionTime=" << executionTime
            << ", availableStorage=" << availableStorage << " MB" << endl;

    sendTaskCompletion(completedTask, true, TASK_OK, "", executionTime);

    busy = false;
    activeTask = QueuedTask();
    tryStartNextTask();
}

void ComputeNodeApp::sendTaskCompletion(const QueuedTask& task, bool success, int failureCode, const std::string& failureReason, double executionTime)
{
    if (task.userNodeAddress.isUnspecified()) {
        EV_WARN << "ComputeNode" << computeNodeId << " cannot send TASK_COMPLETION: user node address is unspecified." << endl;
        return;
    }

    auto payload = makeShared<TaskCompletionMsg>();
    payload->setUserId(task.userId);
    payload->setTaskId(task.taskId);
    payload->setComputeNodeAddress(localAddress);
    payload->setComputeNodePort(localPort);
    payload->setUserNodeAddress(task.userNodeAddress);
    payload->setCompletionTime(simTime());
    payload->setSuccess(success);
    payload->setFailureCode(failureCode);
    payload->setFailureReason(failureReason.c_str());
    payload->setTaskResult(success ? task.transferAmount : 0);
    payload->setExecutionTime(executionTime);

    Packet *pkt = new Packet("TASK_COMPLETION");
    pkt->insertAtBack(payload);
    socket.sendTo(pkt, task.userNodeAddress, userNodePort);

    EV_INFO << "ComputeNode" << computeNodeId << " sent " << (success ? "successful" : "failed")
            << " TASK_COMPLETION for task (" << task.userId << "," << task.taskId
            << ") to " << task.userNodeAddress << ":" << userNodePort << endl;
}

void ComputeNodeApp::stripCpnPathHeader(Packet *packet)
{
    Ptr<const CpnPathHeader> pathHeader;
    auto frontChunk = packet->peekAtFront<Chunk>(b(-1), OPTIONAL_CPN_PATH_HEADER_PEEK_FLAGS);
    pathHeader = dynamicPtrCast<const CpnPathHeader>(frontChunk);

    if (pathHeader == nullptr) {
        try {
            pathHeader = packet->peekAtFront<CpnPathHeader>(b(-1), OPTIONAL_CPN_PATH_HEADER_TYPED_FLAGS);
        }
        catch (const cRuntimeError& e) {
            EV_DEBUG << "ComputeNodeApp: CpnPathHeader is not present at UDP payload front: " << e.what() << endl;
            return;
        }
    }

    if (pathHeader == nullptr)
        return;

    int mode = pathHeader->getMode();
    if (mode != PATH_RECORD_MODE && mode != PATH_USE_MODE) {
        EV_WARN << "ComputeNodeApp: Ignoring invalid CpnPathHeader mode " << mode << endl;
        return;
    }

    packet->eraseAtFront(pathHeader->getChunkLength());
    EV_INFO << "ComputeNodeApp: Stripped CpnPathHeader before processing TASK_DATA." << endl;
}

void ComputeNodeApp::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
}

void ComputeNodeApp::socketClosed(UdpSocket *socket)
{
}

void ComputeNodeApp::finish()
{
}

void ComputeNodeApp::handleStartOperation(LifecycleOperation *operation)
{
}

void ComputeNodeApp::handleStopOperation(LifecycleOperation *operation)
{
    socket.close();
}

void ComputeNodeApp::handleCrashOperation(LifecycleOperation *operation)
{
    socket.destroy();
}

} /* namespace inet */
