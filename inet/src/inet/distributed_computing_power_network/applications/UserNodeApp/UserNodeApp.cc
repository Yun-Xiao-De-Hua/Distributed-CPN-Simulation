#include "inet/networklayer/common/L3AddressResolver.h"
#include <string>
#include "UserNodeApp.h"

Define_Module(inet::UserNodeApp);

namespace inet {

UserNodeApp::UserNodeApp() {
    // TODO Auto-generated constructor stub

}

UserNodeApp::~UserNodeApp() {
    cancelAndDelete(selfTaskCreationEvent);
}

void UserNodeApp::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
       this->userNodeId = par("userNodeId");
       this->localPort = par("localPort");
       this->localAddress = L3AddressResolver().resolve(par("localAddress"));
       this->userGatewayAddress = L3AddressResolver().resolve(par("userGatewayAddress"));
       this->userGatewayPort = par("userGatewayPort");
       this->maxTransmissionBandwidth = par("maxTransmissionBandwidth");
       this->taskId = par("taskId");
       this->taskComputingType = par("taskComputingType");
       this->taskRequiredStorage = par("taskRequiredStorage");
       this->taskDataSize = par("taskDataSize");
       this->taskComputingAmount = par("taskComputingAmount");
       this->taskDelayTolerance = SimTime(par("taskDelayTolerance").doubleValue());
       this->taskBudget = par("taskBudget");

       this->selfTaskCreationEvent = new cMessage("TaskCreationSelfMsg");
    }

    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        socket.setOutputGate(gate("socketOut"));
        socket.bind(localAddress,localPort);
        socket.setCallback(this);   // 将当前应用实例注册为 socket 的回调处理对象

        const double warmTime = 50;
        scheduleAt(simTime() + warmTime, selfTaskCreationEvent);
    }
}

void UserNodeApp::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        if (strcmp(msg->getName(), "TaskCreationSelfMsg") == 0) {
            EV_INFO << "Received TaskCreationSelfMsg" << std::endl;
            sendTaskRequest();
            if (msg == selfTaskCreationEvent)
                selfTaskCreationEvent = nullptr;
            delete msg;
        }
    }
    else {
        // 处理底层传上来的 UDP 数据
        socket.processMessage(msg);
    }
}

void UserNodeApp::sendTaskRequest()
{
    EV_INFO << "User" << userNodeId << " is to send TASK_REQUEST...\n";

    auto payload = makeShared<TaskRequestMsg>();
    // 当前示例场景仅生成一条任务，请求字段需要与后续 CPRP_CONFIRM/TASK_DATA 保持一致。
    payload->setUserId(this->userNodeId);
    payload->setTaskId(this->taskId);
    payload->setGenerationTime(simTime());
    payload->setComputingType(this->taskComputingType);
    payload->setRequiredStorage(this->taskRequiredStorage);
    payload->setComputingAmount(this->taskComputingAmount);
    payload->setTransferAmount(this->taskDataSize);
    payload->setTotalDelayRequirement(this->taskDelayTolerance);
    payload->setBudget(this->taskBudget);
    payload->setUserMaxBandwidth(this->maxTransmissionBandwidth / 1e6);

    UserTaskContext taskContext;
    taskContext.generationTime = payload->getGenerationTime();
    taskContext.computingType = payload->getComputingType();
    taskContext.requiredStorage = payload->getRequiredStorage();
    taskContext.computingAmount = payload->getComputingAmount();
    taskContext.transferAmount = payload->getTransferAmount();
    taskContext.totalDelayRequirement = payload->getTotalDelayRequirement();
    taskContext.budget = payload->getBudget();
    taskContext.userMaxBandwidth = payload->getUserMaxBandwidth();
    taskContextCache[payload->getTaskId()] = taskContext;

    std::string messageType = payload->getMsgType();
    Packet *pkt = new Packet(messageType.c_str());
    pkt->insertAtBack(payload);

    socket.sendTo(pkt, userGatewayAddress, userGatewayPort);

    EV_INFO << "User" << this->userNodeId << " has sent TASK_REQUEST for task(" << this->userNodeId << "-" << payload->getTaskId() << ")\n";
}

void UserNodeApp::sendCprpConfirm(Packet *packet)
{
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(packet) << std::endl;

    // 提取任务请求负载信息
    const auto& sumInfo = packet->popAtFront<RespSummaryMsg>();

    if(sumInfo == nullptr){
        EV_WARN << "Error: Received a Packet named '" << packet->getName()
                 << "', but it does not contain a RESP_SUMMARY payload. Discarding.";
        delete packet;
        return;
    }

    if (sumInfo->getCandidateInfoArraySize() == 0) {
        EV_WARN << "RESP_SUMMARY contains no candidate information." << std::endl;
        delete packet;
        return;
    }

    auto taskIt = taskContextCache.find(sumInfo->getTaskId());
    if (taskIt == taskContextCache.end()) {
        EV_WARN << "No cached task context found for taskId=" << sumInfo->getTaskId() << std::endl;
        delete packet;
        return;
    }

    simtime_t userAccessOneWayDelay = simTime() - sumInfo->getSendTime();
    if (userAccessOneWayDelay < SIMTIME_ZERO) {
        EV_WARN << "RESP_SUMMARY sendTime is later than UserNode receive time; user access RTT is clamped to 0." << endl;
        userAccessOneWayDelay = SIMTIME_ZERO;
    }
    simtime_t userAccessRtt = userAccessOneWayDelay * 2;

    int selectedPathIndex = 0;
    const computeCandidateInfo *selectedCandidate = nullptr;
    simtime_t selectedTotalDelay = SIMTIME_ZERO;
    for (int i = 0; i < (int)sumInfo->getCandidateInfoArraySize(); i++) {
        const auto& candidate = sumInfo->getCandidateInfo(i);
        simtime_t candidateTotalDelay = candidate.pathInfo.totalDelay + userAccessRtt;

        EV_INFO << "User" << userNodeId << " candidate[" << i << "] delay evaluation: "
                << "gatewayToComputeTotalDelay=" << candidate.pathInfo.totalDelay
                << ", userAccessRtt=" << userAccessRtt
                << ", totalDelay=" << candidateTotalDelay
                << ", delayTolerance=" << taskIt->second.totalDelayRequirement
                << ", eligible=" << (candidateTotalDelay < taskIt->second.totalDelayRequirement ? "true" : "false") << endl;

        if (candidateTotalDelay >= taskIt->second.totalDelayRequirement)
            continue;

        if (selectedCandidate == nullptr ||
            candidateTotalDelay < selectedTotalDelay ||
            (candidateTotalDelay == selectedTotalDelay && candidate.nodeInfo.computeCost < selectedCandidate->nodeInfo.computeCost))
        {
            selectedCandidate = &candidate;
            selectedPathIndex = i;
            selectedTotalDelay = candidateTotalDelay;
        }
    }

    if (selectedCandidate == nullptr) {
        EV_WARN << "RESP_SUMMARY candidate selection failed: no candidate satisfies end-to-end delay tolerance." << std::endl;
        delete packet;
        return;
    }

    const auto& selectedNode = selectedCandidate->nodeInfo;

    EV_INFO << "User" << userNodeId << " selected CPRP route from RESP_SUMMARY: selectedPathIndex="
            << selectedPathIndex
            << ", nodeId="
            << selectedNode.computeNodeId
            << ", nodeAddress=" << selectedNode.computeNodeAddress
            << ", nodePort=" << selectedNode.computeNodePort
            << ", computeCost=" << selectedNode.computeCost
            << ", gatewayToComputeTotalDelay=" << selectedCandidate->pathInfo.totalDelay
            << ", userAccessRtt=" << userAccessRtt
            << ", totalDelay=" << selectedTotalDelay
            << ", reservedBandwidth=" << selectedCandidate->pathInfo.reservedBandwidth
            << ", sidPath=[" << selectedCandidate->pathInfo.sidPath << "]" << endl;

    // CPRP_CONFIRM 只通告最终选定的节点和路径索引，任务参数由用户网关侧缓存恢复。
    auto payload = makeShared<CprpConfirmMsg>();
    payload->setUserId(sumInfo->getUserId());
    payload->setTaskId(sumInfo->getTaskId());
    payload->setSelectedNodeId(selectedNode.computeNodeId);
    payload->setSelectedNodeAddress(selectedNode.computeNodeAddress);
    payload->setSelectedNodePort(selectedNode.computeNodePort);
    payload->setSelectedPathIndex(selectedPathIndex);

    std::string messageType = payload->getMsgType();
    Packet *pkt = new Packet(messageType.c_str());
    pkt->insertAtBack(payload);

    socket.sendTo(pkt, userGatewayAddress, userGatewayPort);

    EV_INFO << "User node has sent CPRP_CONFIRM for task(" << sumInfo->getUserId()
            << "," << sumInfo->getTaskId() << ") using selectedNodeId="
            << selectedNode.computeNodeId
            << ", selectedNodeAddress=" << selectedNode.computeNodeAddress
            << ", selectedNodePort=" << selectedNode.computeNodePort
            << ", selectedPathIndex=" << selectedPathIndex << "\n";

    delete packet;
}

void UserNodeApp::processTaskCompletion(Packet *packet)
{
    const auto& completion = packet->popAtFront<TaskCompletionMsg>();
    if (completion == nullptr) {
        EV_WARN << "Error: Received a Packet named '" << packet->getName()
                << "', but it does not contain a TaskCompletionMsg chunk. Discarding.";
        delete packet;
        return;
    }

    int taskId = completion->getTaskId();
    if (completion->getSuccess()) {
        EV_INFO << "User" << userNodeId << " received successful TASK_COMPLETION for task("
                << completion->getUserId() << "," << taskId << ") from "
                << completion->getComputeNodeAddress()
                << ", executionTime=" << completion->getExecutionTime()
                << ", resultSize=" << completion->getTaskResult() << " MB" << endl;
    }
    else {
        EV_WARN << "User" << userNodeId << " received failed TASK_COMPLETION for task("
                << completion->getUserId() << "," << taskId << ") from "
                << completion->getComputeNodeAddress()
                << ", failureCode=" << completion->getFailureCode()
                << ", reason=" << completion->getFailureReason() << endl;
    }

    taskContextCache.erase(taskId);
    delete packet;
}

// UdpSocket::ICallback
void UserNodeApp::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    if(strcmp(packet->getName(), "RESP_SUMMARY") == 0){
        sendCprpConfirm(packet);
    }
    else if(strcmp(packet->getName(), "TASK_COMPLETION") == 0){
        processTaskCompletion(packet);
    }
    else{
        EV_WARN << "Unknown packet type: " << packet->getName() << endl;
        delete packet;
    }
}

void UserNodeApp::socketErrorArrived(UdpSocket *socket, Indication *indication)
{

}

void UserNodeApp::socketClosed(UdpSocket *socket)
{

}

void UserNodeApp::finish()
{

}

void UserNodeApp::handleStartOperation(LifecycleOperation *operation)
{
}

void UserNodeApp::handleStopOperation(LifecycleOperation *operation)
{
    socket.close();
}

void UserNodeApp::handleCrashOperation(LifecycleOperation *operation)
{
    socket.destroy();
}

} /* namespace inet */
