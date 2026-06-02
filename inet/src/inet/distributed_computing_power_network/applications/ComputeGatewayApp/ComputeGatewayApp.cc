
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/InterfaceTable.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/distributed_computing_power_network/message/CpnPathHeader_m.h"
#include <algorithm>
#include <limits>
#include <string>
#include <sstream>
#include "ComputeGatewayApp.h"

Define_Module(inet::ComputeGatewayApp);

namespace inet {

ComputeGatewayApp::ComputeGatewayApp() {
}

ComputeGatewayApp::~ComputeGatewayApp() {
}

void ComputeGatewayApp::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        this->computeGatewayId = par("computeGatewayId");
        this->localPort = par("localPort");
        this->localAddress = L3AddressResolver().resolve(par("localAddress"));
        this->userGatewayPort = par("userGatewayPort");
        this->computeNodePort = par("computeNodePort");

        this->cibInfoMap.clear();
        this->cibUpdateInterval = SimTime(par("cibUpdateInterval").doubleValue());

        this->SelfCibUpdateEvent = new cMessage("CibUpdateSelfMsg");
    }

    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        socket.setOutputGate(gate("socketOut"));
        socket.bind(localPort); // 监听本机所有网卡
        socket.setCallback(this);   // 将当前应用实例注册为 socket 的回调处理对象
        socket.setMulticastLoop(false);

        parseMulticastRoutes(par("multicastRoutes"));

        for (auto& entry : multicastRoutesMap) {
            EV_INFO << "Multicast group " << entry.first << " -> interfaceIds: ";
            for (int ifId : entry.second) {
                EV_INFO << ifId << " ";
            }
            EV_INFO << endl;
        }

        // 加入算力组
        const char *groups = par("multicastGroups").stringValue(); // 获取参数字符串
        cStringTokenizer tokenizer(groups, " "); // 以空格为分隔符

        const char *token;
        while ((token = tokenizer.nextToken()) != nullptr) {
            L3Address multicastGroup = L3AddressResolver().resolve(token);
            socket.joinMulticastGroup(multicastGroup);
            EV_INFO << "Joined group: " << token << endl;

            computingPowerGroup.push_back(multicastGroup);  // 记录算力组 组播地址，用于后续轮询
        }

        // CIB更新计时器
        scheduleAt(simTime(), SelfCibUpdateEvent);
    }
}

void ComputeGatewayApp::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        if (strcmp(msg->getName(), "CibUpdateSelfMsg") == 0) {
            EV_INFO << "Received CibUpdateSelfMsg. To Send CGMP_Query for CIB updating..." << std::endl;
            sendCgmpQuery();

            // 周期性轮询
            SelfCibUpdateEvent = new cMessage("CibUpdateSelfMsg");
            scheduleAt(simTime() + this->cibUpdateInterval, SelfCibUpdateEvent);
            delete msg;
        }
    }
    else {
        if (msg->getArrivalGate() == gate("cprpControlIn")) {
            processCprpCancel(check_and_cast<Packet *>(msg));
        }
        else {
            // 处理底层传上来的 UDP 数据
            socket.processMessage(msg);
        }
    }
}

// 发送组成员查询消息
void ComputeGatewayApp::sendCgmpQuery()
{
    EV_INFO << "Start Sending CGMP_Query for CIB updating..." << std::endl;

    for(const auto& cpGroupAddress : computingPowerGroup){
        auto routeIt = multicastRoutesMap.find(cpGroupAddress);

        if (routeIt == multicastRoutesMap.end() || routeIt->second.empty()) {
            EV_WARN << "No forwarding interfaces configured for multicast group " << cpGroupAddress
                    << ", using default sendTo" << endl;
            auto payload = makeShared<CgmpQueryMsg>();
            payload->setSendTime(simTime());
            std::string messageType = payload->getMsgType();
            Packet *pkt = new Packet(messageType.c_str());
            pkt->insertAtBack(payload);
            socket.sendTo(pkt, cpGroupAddress, computeNodePort);
            continue;
        }

        const std::vector<int>& interfaceIds = routeIt->second;

        EV_INFO << "Forwarding CGMP_Query to multicast group " << cpGroupAddress
                << " via " << interfaceIds.size() << " interface(s)" << endl;

        for (int interfaceId : interfaceIds) {
            auto payload = makeShared<CgmpQueryMsg>();
            payload->setSendTime(simTime());
            std::string messageType = payload->getMsgType();
            Packet *pkt = new Packet(messageType.c_str());
            pkt->insertAtBack(payload);

            auto interfaceReq = pkt->addTagIfAbsent<InterfaceReq>();
            interfaceReq->setInterfaceId(interfaceId);

            socket.sendTo(pkt, cpGroupAddress, computeNodePort);

            EV_INFO << "CGMP_Query sent to group " << cpGroupAddress
                    << " via interfaceId=" << interfaceId << endl;
        }
    }
}

void ComputeGatewayApp::logCurrentCib() const
{
    EV_INFO << "Current CIB on ComputeGateway" << computeGatewayId << ":" << std::endl;

    if (cibInfoMap.empty()) {
        EV_INFO << "  <empty>" << std::endl;
        return;
    }

    std::vector<int> computingTypes;
    for (const auto& typeEntry : cibInfoMap)
        computingTypes.push_back(typeEntry.first);
    std::sort(computingTypes.begin(), computingTypes.end());

    for (int computingType : computingTypes) {
        const auto& groupMap = cibInfoMap.at(computingType);
        std::vector<int> nodeIds;
        for (const auto& nodeEntry : groupMap)
            nodeIds.push_back(nodeEntry.first);
        std::sort(nodeIds.begin(), nodeIds.end());

        for (int nodeId : nodeIds) {
            const CIB& cib = groupMap.at(nodeId);
            EV_INFO << "  CIBEntry{"
                    << "computingType=" << cib.computingType
                    << ", nodeId=" << cib.nodeId
                    << ", serviceGroupAddress=" << cib.serviceGroupAddress
                    << ", address=" << cib.nodeAddress
                    << ", port=" << cib.nodePort
                    << ", interfaceId=" << cib.interfaceId
                    << ", computingCapacity=" << cib.computingCapacity << " FLOPs/s"
                    << ", computeCost=" << cib.computeCost << " CNY/s"
                    << ", availableStorage=" << cib.availableStorage << " MB"
                    << ", maxNetworkBandwidth=" << cib.maxNetworkBandwidth << " Mbps"
                    << ", queueingTime=" << cib.queueingTime
                    << ", querySendTime=" << cib.querySendTime
                    << ", reportSendTime=" << cib.reportSendTime
                    << ", networkDelay=" << cib.networkDelayMs << " ms"
                    << ", updateTime=" << cib.updateTime
                    << "}" << std::endl;
        }
    }
}

void ComputeGatewayApp::logTaskQueueStates() const
{
    EV_INFO << "Current task queue soft states on ComputeGateway" << computeGatewayId << ":" << std::endl;

    if (taskQueueStateMap.empty()) {
        EV_INFO << "  <empty>" << std::endl;
        return;
    }

    for (const auto& entry : taskQueueStateMap) {
        const TaskQueueState& queueState = entry.second;
        EV_INFO << "  TaskQueueState{"
                << "queueId=" << queueState.queueId
                << ", computingType=" << entry.first.first
                << ", computeNodeId=" << queueState.computeNodeId
                << ", serviceGroupAddress=" << queueState.serviceGroupAddress
                << ", computeNodeAddress=" << queueState.computeNodeAddress
                << ", computeNodePort=" << queueState.computeNodePort
                << ", queueTotalTime=" << queueState.queueTotalTime
                << ", taskCount=" << queueState.taskQueue.size()
                << "}" << std::endl;

        for (const auto& task : queueState.taskQueue) {
            EV_INFO << "    ReservedTask{"
                    << "userId=" << task.userId
                    << ", taskId=" << task.taskId
                    << ", userNodeAddress=" << task.userNodeAddress
                    << ", userNodePort=" << task.userNodePort
                    << ", remainingExecutionTime=" << task.remainingExecutionTime
                    << ", reportedByComputeNode=" << (task.reportedByComputeNode ? "true" : "false")
                    << "}" << std::endl;
        }
    }
}

ComputeGatewayApp::TaskQueueState& ComputeGatewayApp::getOrCreateTaskQueueState(const CIB& cib)
{
    auto key = std::make_pair(cib.computingType, cib.nodeId);
    TaskQueueState& queueState = taskQueueStateMap[key];
    if (queueState.queueId < 0)
        queueState.queueId = nextQueueId++;

    queueState.computeNodeId = cib.nodeId;
    queueState.serviceGroupAddress = cib.serviceGroupAddress;
    queueState.computeNodeAddress = cib.nodeAddress;
    queueState.computeNodePort = cib.nodePort;
    if (queueState.queueTotalTime < SIMTIME_ZERO)
        queueState.queueTotalTime = SIMTIME_ZERO;
    return queueState;
}

const ComputeGatewayApp::TaskQueueState *ComputeGatewayApp::findTaskQueueState(int computingType, int computeNodeId) const
{
    auto it = taskQueueStateMap.find({computingType, computeNodeId});
    return it == taskQueueStateMap.end() ? nullptr : &it->second;
}

simtime_t ComputeGatewayApp::getReservedQueueingTime(int computingType, int computeNodeId) const
{
    const TaskQueueState *queueState = findTaskQueueState(computingType, computeNodeId);
    return queueState == nullptr ? SIMTIME_ZERO : queueState->queueTotalTime;
}

bool ComputeGatewayApp::reserveTaskQueueItem(const CprpRequestMsg& requestInfo, const CandidateEvaluation& selectedCandidate)
{
    const CIB& cib = selectedCandidate.cib;
    TaskQueueState& queueState = getOrCreateTaskQueueState(cib);

    for (const auto& task : queueState.taskQueue) {
        if (task.userId == requestInfo.getUserId() && task.taskId == requestInfo.getTaskId()) {
            EV_WARN << "Task queue reservation already exists for task (" << task.userId << "," << task.taskId
                    << ") on queueId=" << queueState.queueId << std::endl;
            return false;
        }
    }

    ReservedTaskItem task;
    task.userId = requestInfo.getUserId();
    task.taskId = requestInfo.getTaskId();
    task.userNodeAddress = requestInfo.getUserNodeAddress();
    task.userNodePort = requestInfo.getUserNodePort();
    double computationDelay = cib.computingCapacity > 0 ? requestInfo.getComputingAmount() / cib.computingCapacity : 0;
    task.remainingExecutionTime = SimTime(computationDelay);

    queueState.taskQueue.push_back(task);
    queueState.queueTotalTime += task.remainingExecutionTime;

    EV_INFO << "Reserved compute task queue item: queueId=" << queueState.queueId
            << ", task=(" << task.userId << "," << task.taskId << ")"
            << ", computeNode=" << queueState.computeNodeId << "@" << queueState.computeNodeAddress << ":" << queueState.computeNodePort
            << ", serviceGroupAddress=" << queueState.serviceGroupAddress
            << ", remainingExecutionTime=" << task.remainingExecutionTime
            << ", queueTotalTime=" << queueState.queueTotalTime << std::endl;
    return true;
}

bool ComputeGatewayApp::removeReservedTaskQueueItem(int userId, int taskId, const L3Address& computeNodeAddress, int computeNodePort, const char *reason)
{
    for (auto& entry : taskQueueStateMap) {
        TaskQueueState& queueState = entry.second;
        if (!computeNodeAddress.isUnspecified() && queueState.computeNodeAddress != computeNodeAddress)
            continue;
        if (computeNodePort > 0 && queueState.computeNodePort != computeNodePort)
            continue;

        for (auto it = queueState.taskQueue.begin(); it != queueState.taskQueue.end(); ++it) {
            if (it->userId == userId && it->taskId == taskId) {
                simtime_t removedTime = it->remainingExecutionTime;
                queueState.taskQueue.erase(it);
                queueState.queueTotalTime -= removedTime;
                if (queueState.queueTotalTime < SIMTIME_ZERO)
                    queueState.queueTotalTime = SIMTIME_ZERO;
                EV_INFO << "Removed compute task queue item by " << reason
                        << ": queueId=" << queueState.queueId
                        << ", task=(" << userId << "," << taskId << ")"
                        << ", removedTime=" << removedTime
                        << ", queueTotalTime=" << queueState.queueTotalTime << std::endl;
                return true;
            }
        }
    }

    EV_WARN << "No matching compute task queue item found by " << reason
            << " for task (" << userId << "," << taskId << ")"
            << ", computeNodeAddress=" << computeNodeAddress
            << ", computeNodePort=" << computeNodePort << std::endl;
    return false;
}

void ComputeGatewayApp::reconcileTaskQueueWithReport(const CIB& cib, const Ptr<const CgmpReportMsg>& reportInfo)
{
    TaskQueueState& queueState = getOrCreateTaskQueueState(cib);

    for (auto it = queueState.taskQueue.begin(); it != queueState.taskQueue.end(); ) {
        bool reported = false;
        for (int i = 0; i < (int)reportInfo->getTaskStateArraySize(); i++) {
            const cgmpTaskState& taskState = reportInfo->getTaskState(i);
            if (it->userId == taskState.userId && it->taskId == taskState.taskId) {
                it->remainingExecutionTime = taskState.remainingExecutionTime;
                it->reportedByComputeNode = true;
                reported = true;
                break;
            }
        }

        if (!reported && it->reportedByComputeNode) {
            EV_INFO << "CGMP_Report indicates task has left compute node queue, removing soft state item: queueId=" << queueState.queueId
                    << ", task=(" << it->userId << "," << it->taskId << ")" << std::endl;
            it = queueState.taskQueue.erase(it);
        }
        else {
            ++it;
        }
    }

    queueState.queueTotalTime = SIMTIME_ZERO;
    for (const auto& task : queueState.taskQueue)
        queueState.queueTotalTime += task.remainingExecutionTime;

    EV_INFO << "Reconciled task queue soft state by CGMP_Report: queueId=" << queueState.queueId
            << ", reportedTaskCount=" << reportInfo->getTaskStateArraySize()
            << ", queueTotalTime=" << queueState.queueTotalTime << std::endl;
}

void ComputeGatewayApp::processCprpCancel(Packet *packet)
{
    const auto& cancelInfo = packet->popAtFront<CancelMsg>();
    if (cancelInfo == nullptr) {
        EV_WARN << "Error: Received a Packet named '" << packet->getName()
                << "', but it does not contain a CancelMsg chunk. Discarding.";
        delete packet;
        return;
    }

    removeReservedTaskQueueItem(cancelInfo->getUserId(), cancelInfo->getTaskId(), cancelInfo->getComputeNodeAddress(), cancelInfo->getComputeNodePort(), "CPRP_CANCEL");
    logTaskQueueStates();
    delete packet;
}

std::vector<ComputeGatewayApp::CandidateEvaluation> ComputeGatewayApp::evaluateCandidateNodes(const CprpRequestMsg& requestInfo, const std::unordered_map<int, CIB>& groupMap)
{
    std::vector<CandidateEvaluation> candidates;
    IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);

    double userMaxBandwidthMbps = requestInfo.getUserMaxBandwidth();

    std::vector<int> nodeIds;
    for (const auto& entry : groupMap)
        nodeIds.push_back(entry.first);
    std::sort(nodeIds.begin(), nodeIds.end());

    EV_INFO << "Candidate RTT evaluation for task(" << requestInfo.getUserId() << "," << requestInfo.getTaskId()
            << ") userNode=" << requestInfo.getUserNodeAddress()
            << ", propagationDelayScope=computeGatewayToComputeNodeRttOnly" << std::endl;

    for (int nodeId : nodeIds) {
        const CIB& cib = groupMap.at(nodeId);
        CandidateEvaluation evaluation;
        evaluation.cib = cib;

        NetworkInterface *nodeInterface = ift != nullptr && cib.interfaceId >= 0 ? ift->getInterfaceById(cib.interfaceId) : nullptr;
        double gatewayLinkBandwidthBps = nodeInterface != nullptr ? nodeInterface->getDatarate() : 0;
        double gatewayLinkBandwidthMbps = gatewayLinkBandwidthBps / 1e6;

        double transmissionDelay = userMaxBandwidthMbps > 0 ? requestInfo.getTransferAmount() * 8.0 / userMaxBandwidthMbps : std::numeric_limits<double>::infinity();
        double propagationDelay = cib.networkDelayMs / 1000.0;
        double computationDelay = cib.computingCapacity > 0 ? requestInfo.getComputingAmount() / cib.computingCapacity : std::numeric_limits<double>::infinity();
        double queueingDelay = getReservedQueueingTime(cib.computingType, cib.nodeId).dbl();
        evaluation.totalDelay = transmissionDelay + propagationDelay + computationDelay + queueingDelay;

        if (cib.computingType != requestInfo.getComputingType())
            evaluation.rejectReason = "computing type mismatch";
        else if (requestInfo.getTransferAmount() >= cib.availableStorage)
            evaluation.rejectReason = "task data size exceeds available storage";
        else if (gatewayLinkBandwidthBps <= 0)
            evaluation.rejectReason = "gateway link bandwidth is unavailable";
        else if (gatewayLinkBandwidthMbps < userMaxBandwidthMbps)
            evaluation.rejectReason = "gateway link bandwidth is lower than user maximum bandwidth";
        else if (cib.maxNetworkBandwidth < userMaxBandwidthMbps)
            evaluation.rejectReason = "compute node network bandwidth is lower than user maximum bandwidth";
        else if (evaluation.totalDelay >= requestInfo.getTotalDelayRequirement().dbl())
            evaluation.rejectReason = "total delay exceeds user delay tolerance";
        else
            evaluation.eligible = true;

        candidates.push_back(evaluation);

        EV_INFO << "  Candidate{"
                << "nodeId=" << cib.nodeId
                << ", queueId=" << getOrCreateTaskQueueState(cib).queueId
                << ", serviceGroupAddress=" << cib.serviceGroupAddress
                << ", address=" << cib.nodeAddress
                << ", port=" << cib.nodePort
                << ", interfaceId=" << cib.interfaceId
                << ", computingCapacity=" << cib.computingCapacity << " FLOPs/s"
                << ", softStateQueueingTime=" << queueingDelay << " s"
                << ", computeCost=" << cib.computeCost << " CNY/s"
                << ", availableStorage=" << cib.availableStorage << " MB"
                << ", maxNetworkBandwidth=" << cib.maxNetworkBandwidth << " Mbps"
                << ", gatewayLinkBandwidth=" << gatewayLinkBandwidthMbps << " Mbps"
                << ", computeGatewayToComputeNodeRtt=" << cib.networkDelayMs << " ms"
                << ", transmissionDelay=" << transmissionDelay << " s"
                << ", propagationDelay=" << propagationDelay << " s"
                << ", computationDelay=" << computationDelay << " s"
                << ", queueingDelay=" << queueingDelay << " s"
                << ", totalDelay=" << evaluation.totalDelay << " s"
                << ", delayTolerance=" << requestInfo.getTotalDelayRequirement() << " s"
                << ", eligible=" << (evaluation.eligible ? "true" : "false")
                << (evaluation.eligible ? "" : (std::string(", rejectReason=") + evaluation.rejectReason))
                << "}" << std::endl;
    }

    return candidates;
}

bool ComputeGatewayApp::selectBestCandidate(const std::vector<CandidateEvaluation>& candidates, CandidateEvaluation& bestCandidate) const
{
    bool found = false;
    double bestDelay = std::numeric_limits<double>::infinity();

    for (const auto& candidate : candidates) {
        if (!candidate.eligible)
            continue;
        if (candidate.totalDelay < bestDelay) {
            bestDelay = candidate.totalDelay;
            bestCandidate = candidate;
            found = true;
        }
    }

    if (found) {
        EV_INFO << "Selected compute node candidate: nodeId=" << bestCandidate.cib.nodeId
                << ", address=" << bestCandidate.cib.nodeAddress
                << ", port=" << bestCandidate.cib.nodePort
                << ", totalDelay=" << bestCandidate.totalDelay << " s" << std::endl;
    }
    else {
        EV_WARN << "No eligible compute node candidate found." << std::endl;
    }

    return found;
}

// 更新CIB
void ComputeGatewayApp::updateCib(Packet *packet)
{
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(packet) << std::endl
            << "Start updating CIB...\n";

    // 提取算力组成员应答信息
    const auto& reportInfo = packet->popAtFront<CgmpReportMsg>();

    if(reportInfo == nullptr){
        EV_WARN << "Error: Received a Packet named '" << packet->getName()
                 << "', but it does not contain a CgmpReportMsg chunk. Discarding.";
        delete packet;
        return;
    }

    simtime_t receiveTime = simTime();
    simtime_t querySendTime = reportInfo->getQuerySendTime();
    double networkDelayMs = 0;
    if (receiveTime < querySendTime) {
        EV_WARN << "CGMP_Report has a future querySendTime=" << querySendTime
                << ", receiveTime=" << receiveTime
                << "; networkDelay is clamped to 0 ms." << std::endl;
    }
    else {
        networkDelayMs = (receiveTime - querySendTime).dbl() * 1000.0;
    }

    // 算力组 CIB 更新
    auto& groupMap = cibInfoMap[reportInfo->getComputingType()];
    CIB& cib = groupMap[reportInfo->getComputeNodeId()];
    cib.nodeId = reportInfo->getComputeNodeId();
    cib.computingType = reportInfo->getComputingType();
    cib.nodeAddress = reportInfo->getComputeNodeAddress();
    cib.nodePort = reportInfo->getComputeNodePort();
    cib.serviceGroupAddress = reportInfo->getServiceGroupAddress();
    auto interfaceInd = packet->findTag<InterfaceInd>();
    cib.interfaceId = interfaceInd != nullptr ? interfaceInd->getInterfaceId() : -1;
    cib.computingCapacity = reportInfo->getComputingCapacity();
    cib.availableStorage = reportInfo->getAvailableStorage();
    cib.maxNetworkBandwidth = reportInfo->getMaxNetworkBandwidth();
    cib.computeCost = reportInfo->getComputeCost();
    cib.queueingTime = reportInfo->getQueueingTime();
    cib.querySendTime = querySendTime;
    cib.reportSendTime = reportInfo->getSendTime();
    cib.networkDelayMs = networkDelayMs;
    cib.updateTime = receiveTime;

    EV_INFO << "CIB has been updated: computingType=" << cib.computingType
            << ", nodeId=" << cib.nodeId
            << ", address=" << cib.nodeAddress
            << ", port=" << cib.nodePort
            << ", serviceGroupAddress=" << cib.serviceGroupAddress
            << ", interfaceId=" << cib.interfaceId
            << ", computingCapacity=" << cib.computingCapacity << " FLOPs/s"
            << ", computeCost=" << cib.computeCost << " CNY/s"
            << ", availableStorage=" << cib.availableStorage << " MB"
            << ", maxNetworkBandwidth=" << cib.maxNetworkBandwidth << " Mbps"
            << ", queueingTime=" << cib.queueingTime
            << ", networkDelay=" << cib.networkDelayMs << " ms"
            << ", updateTime=" << cib.updateTime << std::endl;

    reconcileTaskQueueWithReport(cib, reportInfo);
    logCurrentCib();
    logTaskQueueStates();

    delete packet;
}

// 发送算力应答
void ComputeGatewayApp::sendCprpResponse(Packet *packet)
{
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(packet) << std::endl;

    // 根据用户网关转发来的 CPRP_REQ 选择一个候选算力节点，构造 CPRP_RESP。
    // 该响应既承担候选节点通告作用，也为后续网络层路径记录与会话建立提供初始输入。
    const auto& requestInfo = packet->popAtFront<CprpRequestMsg>();

    if(requestInfo == nullptr){
        EV_WARN << "Error: Received a Packet named '" << packet->getName()
                 << "', but it does not contain a CprpRequestMsg chunk. Discarding.";
        delete packet;
        return;
    }

    if(cibInfoMap.find(requestInfo->getComputingType()) == cibInfoMap.end()){
        EV_INFO << "ComputeGateway" << computeGatewayId << " has no cib entry for computingType: " << requestInfo->getComputingType() << std::endl;
        delete packet;
        return;
    }

    const auto& groupMap = cibInfoMap.at(requestInfo->getComputingType());

    if (groupMap.empty()) {
        EV_WARN << "ComputeGateway" << computeGatewayId
                << " has an empty CIB group for computingType: "
                << requestInfo->getComputingType() << std::endl;
        delete packet;
        return;
    }

    std::vector<CandidateEvaluation> candidates = evaluateCandidateNodes(*requestInfo, groupMap);
    if (candidates.empty()) {
        EV_WARN << "ComputeGateway" << computeGatewayId << " has no candidate evaluation result for task("
                << requestInfo->getUserId() << "," << requestInfo->getTaskId() << ")." << std::endl;
        delete packet;
        return;
    }

    CandidateEvaluation bestCandidate;
    if (!selectBestCandidate(candidates, bestCandidate)) {
        delete packet;
        return;
    }

    reserveTaskQueueItem(*requestInfo, bestCandidate);

    CIB destNodeInfo = bestCandidate.cib;

    auto payload = makeShared<CprpResponseMsg>();
    payload->setUserId(requestInfo->getUserId());
    payload->setTaskId(requestInfo->getTaskId());
    payload->setComputeNodeId(destNodeInfo.nodeId);

    payload->setComputeNodeAddress(destNodeInfo.nodeAddress);
    payload->setComputeNodePort(destNodeInfo.nodePort);
    payload->setComputingType(destNodeInfo.computingType);
    payload->setComputingCapacity(destNodeInfo.computingCapacity);
    payload->setAvailableStorage(destNodeInfo.availableStorage);
    payload->setMaxNetworkBandwidth(destNodeInfo.maxNetworkBandwidth);
    payload->setSendTime(simTime());

    payload->setRequiredBandwidth(requestInfo->getUserMaxBandwidth() * 1e6);
    payload->setMaxDelayTolerance(requestInfo->getTotalDelayRequirement());
    payload->setComputeCost(destNodeInfo.computeCost);

    payload->setLastHopSendTime(simTime());
    payload->setLastHopAddress(localAddress);
    // 初始累计时延只包含算力网关侧可确定的部分：传输、计算、排队、算力网关到算力节点RTT。
    // RESP 返回用户网关时，网络层会继续动态叠加用户网关到算力网关的路径RTT。
    payload->setAccumulatedDelay(SimTime(bestCandidate.totalDelay));

    std::string messageType = payload->getMsgType();
    Packet *pkt = new Packet(messageType.c_str());
    pkt->insertAtBack(payload);

    auto pathReq = pkt->addTagIfAbsent<CpnPathReq>();
    pathReq->setMode(PATH_RECORD_MODE);
    pathReq->setUserId(requestInfo->getUserId());
    pathReq->setTaskId(requestInfo->getTaskId());
    pathReq->setUserGatewayAddress(requestInfo->getUserGatewayAddress());
    pathReq->setRequiredBandwidth(requestInfo->getUserMaxBandwidth() * 1e6);
    // 应用层只写入算力节点。算力网关及后续路由器的出口接口IP由网络层按InterfaceReq追加。
    pathReq->setHopAddressArraySize(1);
    pathReq->setHopAddress(0, destNodeInfo.nodeAddress);

    auto reservationInterface = pkt->addTagIfAbsent<InterfaceInd>();
    reservationInterface->setInterfaceId(destNodeInfo.interfaceId);

    socket.sendTo(pkt, requestInfo->getUserGatewayAddress(), userGatewayPort);

    EV_INFO << "ComputeGateway" << computeGatewayId << " has sent CPRP_RESP with path recording for task("
            << requestInfo->getUserId() << "," << requestInfo->getTaskId() << ").\n";

    delete packet;
}

// UdpSocket::ICallback
void ComputeGatewayApp::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    if(strcmp(packet->getName(), "CPRP_REQ") == 0){
        sendCprpResponse(packet);
    }
    else if(strcmp(packet->getName(), "CGMP_Report") == 0){
        updateCib(packet);
    }
    else if(strcmp(packet->getName(), "CPRP_CANCEL") == 0){
        processCprpCancel(packet);
    }
    else{
        EV_WARN << "Unknown packet type: " << packet->getName() << std::endl;
        delete packet;
    }
}

void ComputeGatewayApp::socketErrorArrived(UdpSocket *socket, Indication *indication)
{

}

void ComputeGatewayApp::socketClosed(UdpSocket *socket)
{

}

void ComputeGatewayApp::finish()
{

}

void ComputeGatewayApp::handleStartOperation(LifecycleOperation *operation)
{
}

void ComputeGatewayApp::handleStopOperation(LifecycleOperation *operation)
{
    socket.close();
}

void ComputeGatewayApp::handleCrashOperation(LifecycleOperation *operation)
{
    socket.destroy();
}

void ComputeGatewayApp::parseMulticastRoutes(const char *routesStr)
{
    if (!routesStr || strlen(routesStr) == 0) {
        EV_WARN << "No multicast routes configured" << endl;
        return;
    }

    IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
    if (ift == nullptr) {
        EV_ERROR << "Interface table not found!" << endl;
        return;
    }

    std::string routes(routesStr);
    std::istringstream routeStream(routes);
    std::string routeEntry;

    while (std::getline(routeStream, routeEntry, ';')) {
        if (routeEntry.empty()) continue;

        std::istringstream entryStream(routeEntry);
        std::string groupAddrStr, interfacesStr;

        if (entryStream >> groupAddrStr >> interfacesStr) {
            L3Address groupAddr = L3AddressResolver().resolve(groupAddrStr.c_str());
            if (groupAddr.isUnspecified()) {
                EV_ERROR << "Invalid multicast address: " << groupAddrStr << endl;
                continue;
            }

            std::vector<int> interfaceIds;
            std::istringstream ifStream(interfacesStr);
            std::string ifName;

            while (std::getline(ifStream, ifName, ',')) {
                if (ifName.empty()) continue;
                NetworkInterface *ie = ift->findInterfaceByName(ifName.c_str());
                if (ie != nullptr) {
                    interfaceIds.push_back(ie->getInterfaceId());
                    EV_INFO << "Added interface " << ifName
                            << " (id=" << ie->getInterfaceId()
                            << ") for multicast group " << groupAddr << endl;
                } else {
                    EV_ERROR << "Interface '" << ifName << "' not found!" << endl;
                }
            }

            if (!interfaceIds.empty()) {
                multicastRoutesMap[groupAddr] = interfaceIds;
            }
        }
    }
}

} /* namespace inet */
