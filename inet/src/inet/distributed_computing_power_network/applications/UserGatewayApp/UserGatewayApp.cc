#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/InterfaceTable.h"
#include "inet/networklayer/common/DscpTag_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/distributed_computing_power_network/message/CpnPathHeader_m.h"
#include "UserGatewayApp.h"

Define_Module(inet::UserGatewayApp);

namespace inet {

static const int OPTIONAL_CPN_PATH_HEADER_PEEK_FLAGS = Chunk::PF_ALLOW_NULLPTR | Chunk::PF_ALLOW_EMPTY | Chunk::PF_ALLOW_INCOMPLETE;
static const int OPTIONAL_CPN_PATH_HEADER_TYPED_FLAGS = Chunk::PF_ALLOW_NULLPTR | Chunk::PF_ALLOW_INCOMPLETE | Chunk::PF_ALLOW_REINTERPRETATION;

UserGatewayApp::UserGatewayApp() {
    // TODO Auto-generated constructor stub

}

UserGatewayApp::~UserGatewayApp() {
    for (auto& entry : requestTimers)
        cancelAndDelete(entry.second);
    requestTimers.clear();
}

void UserGatewayApp::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
       this->userGatewayId = par("userGatewayId");
       this->localPort = par("localPort");
       this->localAddress = L3AddressResolver().resolve(par("localAddress"));
       this->userNodePort = par("userNodePort");
       this->computeGatewayPort = par("computeGatewayPort");
       this->computeNodePort = par("computeNodePort");
       this->requestTimeout = SimTime(par("requestTimeout").doubleValue());

       this->userNodeIpMap.clear();

       userNodeIpMap[1] = L3AddressResolver().resolve("10.0.0.1");  // test，暂时硬编码
    }

    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        socket.setOutputGate(gate("socketOut"));
        socket.bind(localPort);
        socket.setCallback(this);
        socket.setMulticastLoop(false);
        
        // 解析组播路由配置 (组播地址 -> 接口列表)
        parseMulticastRoutes(par("multicastRoutes"));
        
        // 解析按算力类型配置的组播组地址 (每种算力类型只对应一个组播组地址)
        parseMulticastGroup(par("cpuMulticastGroup"), 0);  // 0 = CPU
        parseMulticastGroup(par("gpuMulticastGroup"), 1);  // 1 = GPU
        
        // 打印配置信息
        for (auto& entry : computingTypeMulticastGroup) {
            EV_INFO << "Computing type " << entry.first << " -> multicast group: " << entry.second << endl;
        }
        
        for (auto& entry : multicastInterfacesMap) {
            EV_INFO << "Multicast group " << entry.first << " -> interfaces: ";
            for (int ifId : entry.second) {
                EV_INFO << ifId << " ";
            }
            EV_INFO << endl;
        }
        
        socket.setTimeToLive(32);
    }
}

void UserGatewayApp::parseMulticastGroup(const char *groupStr, int computingType)
{
    if (groupStr && strlen(groupStr) > 0) {
        L3Address addr = L3AddressResolver().resolve(groupStr);
        if (!addr.isUnspecified()) {
            computingTypeMulticastGroup[computingType] = addr;
            EV_INFO << "Configured multicast group " << addr 
                    << " for computing type " << computingType << endl;
        } else {
            EV_ERROR << "Warning: Invalid multicast address '" << groupStr << "'" << endl;
        }
    }
}

void UserGatewayApp::parseMulticastRoutes(const char *routesStr)
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
    
    // 格式: "组播地址 接口1,接口2,...;组播地址 接口1,..."
    // 例如: "225.0.1.1 ppp1;225.0.1.2 ppp1,ppp2"
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
            
            // 解析接口列表 (逗号分隔)
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
                multicastInterfacesMap[groupAddr] = interfaceIds;
            }
        }
    }
}

void UserGatewayApp::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        if (strcmp(msg->getName(), "RespTimeoutSelfMsg") == 0) {
            RespTimeoutSelfMsg *timer = check_and_cast<RespTimeoutSelfMsg*>(msg);
            int uid = timer->getUserId();
            int tid = timer->getTaskId();
            auto key = std::make_pair(uid, tid);

            auto timerIt = requestTimers.find(key);
            if (timerIt != requestTimers.end() && timerIt->second == timer)
                requestTimers.erase(timerIt);
            expiredRequests.insert(key);

            EV_INFO << "Received RespTimeoutSelfMsg. Sending response summary to user node for task(" << uid << "," << tid << ").\n";
            // 发送可用算力信息至对应用户节点
            sendResponseSummary(uid, tid);
            delete msg;
        }
    }
    else {
        // 处理底层传上来的 UDP 数据
        socket.processMessage(msg);
    }
}

void UserGatewayApp::sendCprpRequest(Packet *packet)
{
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(packet) << std::endl;

    // 用户网关在这里把终端侧任务请求转换为 CPRP_REQ，并缓存原始任务上下文。
    // 后续收到 CPRP_RESP 与 CPRP_CONFIRM 后，应用层需要依赖这份上下文来恢复 TASK_DATA 的完整业务字段。
    const auto& requestInfo = packet->popAtFront<TaskRequestMsg>();

    if(requestInfo == nullptr){
        EV_WARN << "Error: Received a Packet named '" << packet->getName()
                 << "', but it does not contain a TaskRequestMsg chunk. Discarding.";
        delete packet;
        return;
    }

    // 创建算力请求载荷
    L3Address userNodeAddress = userNodeIpMap.at(requestInfo->getUserId());
    simtime_t userAccessOneWayDelay = simTime() - requestInfo->getGenerationTime();
    if (userAccessOneWayDelay < SIMTIME_ZERO) {
        EV_WARN << "TaskRequestMsg generationTime is later than UserGateway receive time; userAccessRtt is clamped to 0." << endl;
        userAccessOneWayDelay = SIMTIME_ZERO;
    }
    simtime_t userAccessRtt = userAccessOneWayDelay * 2;

    auto payload = makeShared<CprpRequestMsg>();
    payload->setUserId(requestInfo->getUserId());
    payload->setTaskId(requestInfo->getTaskId());
    payload->setUserGatewayAddress(localAddress);
    payload->setUserNodeAddress(userNodeAddress);
    payload->setUserNodePort(userNodePort);
    payload->setUserAccessRtt(userAccessRtt);
    payload->setUserGatewayForwardTime(simTime());
    payload->setGenerationTime(requestInfo->getGenerationTime());
    payload->setComputingType(requestInfo->getComputingType());
    payload->setRequiredStorage(requestInfo->getRequiredStorage());
    payload->setComputingAmount(requestInfo->getComputingAmount());
    payload->setTransferAmount(requestInfo->getTransferAmount());
    payload->setTotalDelayRequirement(requestInfo->getTotalDelayRequirement());
    payload->setBudget(requestInfo->getBudget());
    payload->setUserMaxBandwidth(requestInfo->getUserMaxBandwidth());

    RequestContext requestContext;
    requestContext.userNodeAddress = userNodeAddress;
    requestContext.userNodePort = userNodePort;
    requestContext.generationTime = requestInfo->getGenerationTime();
    requestContext.computingType = requestInfo->getComputingType();
    requestContext.requiredStorage = requestInfo->getRequiredStorage();
    requestContext.computingAmount = requestInfo->getComputingAmount();
    requestContext.transferAmount = requestInfo->getTransferAmount();
    requestContext.totalDelayRequirement = requestInfo->getTotalDelayRequirement();
    requestContext.budget = requestInfo->getBudget();
    requestContext.userMaxBandwidth = requestInfo->getUserMaxBandwidth();
    auto requestKey = std::make_pair(requestInfo->getUserId(), requestInfo->getTaskId());
    requestContextCache[requestKey] = requestContext;
    candidateCache.erase(requestKey);
    selectedPathCache.erase(requestKey);
    startCprpRequestTimer(requestInfo->getUserId(), requestInfo->getTaskId());

    std::string messageType = payload->getMsgType();
    int computingType = requestInfo->getComputingType();
    
    // 获取对应算力类型的组播组地址
    auto typeIt = computingTypeMulticastGroup.find(computingType);
    if (typeIt == computingTypeMulticastGroup.end()) {
        EV_ERROR << "No multicast group configured for computing type " << computingType << endl;
        delete packet;
        return;
    }
    
    const inet::L3Address& groupAddr = typeIt->second;
    
    // 获取该组播地址对应的转发接口列表
    auto routeIt = multicastInterfacesMap.find(groupAddr);
    if (routeIt == multicastInterfacesMap.end() || routeIt->second.empty()) {
        EV_ERROR << "No forwarding interfaces configured for multicast group " << groupAddr << endl;
        delete packet;
        return;
    }
    
    const std::vector<int>& interfaceIds = routeIt->second;
    
    EV_INFO << "Forwarding CPRP request for task(" << payload->getUserId() << "-" << payload->getTaskId() 
            << ") computingType=" << computingType 
            << ", userNodeAddress=" << userNodeAddress
            << ", userAccessRtt=" << userAccessRtt
            << ", userGatewayForwardTime=" << payload->getUserGatewayForwardTime()
            << " to multicast group " << groupAddr 
            << " via " << interfaceIds.size() << " interface(s)" << endl;

    // 向每个转发接口复制转发组播数据包
    for (int interfaceId : interfaceIds) {
        Packet *pkt = new Packet(messageType.c_str());
        pkt->insertAtBack(payload);
        
        auto interfaceReq = pkt->addTagIfAbsent<InterfaceReq>();
        interfaceReq->setInterfaceId(interfaceId);
        
        socket.sendTo(pkt, groupAddr, computeGatewayPort);
        
        EV_INFO << "Sent to group " << groupAddr 
                << " via interfaceId=" << interfaceId << endl;
    }

    // 清理任务请求消息
    delete packet;
}

void UserGatewayApp::sendResponseSummary(int uid, int tid)
{
    EV_INFO << "Preparing RespSummaryMsg for user" << uid << " task(" << uid << "," << tid << ").\n";

    auto candidateIt = candidateCache.find({uid, tid});
    if (candidateIt == candidateCache.end()) {
        EV_WARN << "Cannot send RespSummaryMsg for task(" << uid << "," << tid
                << "): candidate cache is missing." << endl;
        return;
    }

    auto& candidates = candidateIt->second;
    size_t candidateCount = candidates.size();
    if (candidateCount == 0) {
        EV_WARN << "Cannot send RespSummaryMsg for task(" << uid << "," << tid
                << "): no candidate entries available." << endl;
        return;
    }

    auto payload = makeShared<RespSummaryMsg>();
    payload->setCandidateInfoArraySize(candidateCount);
    for (size_t i = 0; i < candidateCount; i++) {
        computeCandidateInfo candidate;
        candidate.nodeInfo = candidates[i].nodeInfo;

        std::ostringstream sidPathStream;
        for (size_t j = 0; j < candidates[i].sidPath.size(); j++) {
            if (j > 0)
                sidPathStream << " -> ";
            sidPathStream << candidates[i].sidPath[j];
        }
        std::string sidPath = sidPathStream.str();
        candidate.pathInfo.sidPath = sidPath.c_str();
        candidate.pathInfo.totalDelay = SimTime(candidates[i].totalDelay);
        candidate.pathInfo.reservedBandwidth = candidates[i].reservedBandwidth;
        candidate.pathInfo.timestamp = candidates[i].timestamp;

        payload->setCandidateInfo(i, candidate);

        EV_INFO << "RespSummary candidate[" << i << "]: nodeId=" << candidate.nodeInfo.computeNodeId
                << ", nodeAddress=" << candidate.nodeInfo.computeNodeAddress
                << ", nodePort=" << candidate.nodeInfo.computeNodePort
                << ", totalDelay=" << candidate.pathInfo.totalDelay
                << ", reservedBandwidth=" << candidate.pathInfo.reservedBandwidth
                << " bps, sidPath=[" << candidate.pathInfo.sidPath << "]" << endl;
    }

    payload->setUserId(uid);
    payload->setTaskId(tid);

    std::string messageType = payload->getMsgType();
    Packet *pkt = new Packet(messageType.c_str());
    pkt->insertAtBack(payload);

    L3Address userAddress = userNodeIpMap.at(uid);
    socket.sendTo(pkt, userAddress, userNodePort);

    EV_INFO << "UserGateway" << userGatewayId << " sent RespSummaryMsg with " << candidateCount
            << " candidate(s) to user" << uid << " for task(" << uid << "," << tid << ")." << std::endl;
}

// 启动算力请求计时器
void UserGatewayApp::startCprpRequestTimer(int userId, int taskId)
{
    auto key = std::make_pair(userId, taskId);
    auto existingTimer = requestTimers.find(key);
    if (existingTimer != requestTimers.end()) {
        cancelAndDelete(existingTimer->second);
        requestTimers.erase(existingTimer);
    }
    expiredRequests.erase(key);

    RespTimeoutSelfMsg *timer = new RespTimeoutSelfMsg("RespTimeoutSelfMsg");

    timer->setUserId(userId);
    timer->setTaskId(taskId);

    scheduleAt(simTime() + requestTimeout, timer);
    requestTimers[key] = timer;

    EV_INFO << "Started CPRP request timer for task(" << userId << "," << taskId
            << "), timeout=" << requestTimeout << endl;
}

// 算力应答消息处理
void UserGatewayApp::processCprpResp(Packet *packet)
{
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(packet) << std::endl;

    stripCpnPathHeader(packet);

    const auto& respInfo = packet->popAtFront<CprpResponseMsg>();

    if(respInfo == nullptr){
        EV_WARN << "Error: Received a Packet named '" << packet->getName()
                 << "', but it does not contain a CprpResponseMsg chunk. Discarding.";
        delete packet;
        return;
    }

    int uid = respInfo->getUserId();
    int tid = respInfo->getTaskId();

    if (expiredRequests.find({uid, tid}) != expiredRequests.end()) {
        EV_INFO << "Dropping late CPRP_RESP for expired task(" << uid << "," << tid
                << "); request timer has already fired." << endl;
        delete packet;
        return;
    }

    EV_INFO << "Parsed CPRP_RESP for task(" << uid << "," << tid << "): "
            << "computeNodeId=" << respInfo->getComputeNodeId()
            << ", computeNodeAddress=" << respInfo->getComputeNodeAddress()
            << ", computeNodePort=" << respInfo->getComputeNodePort()
            << ", computingType=" << respInfo->getComputingType()
            << ", computingCapacity=" << respInfo->getComputingCapacity() << " FLOPs/s"
            << ", availableStorage=" << respInfo->getAvailableStorage() << " MB"
            << ", maxNetworkBandwidth=" << respInfo->getMaxNetworkBandwidth() << " Mbps"
            << ", computeCost=" << respInfo->getComputeCost() << " CNY/s"
            << ", requiredBandwidth=" << respInfo->getRequiredBandwidth() << " bps"
            << ", maxDelayTolerance=" << respInfo->getMaxDelayTolerance()
            << ", accumulatedDelay=" << respInfo->getAccumulatedDelay()
            << ", sendTime=" << respInfo->getSendTime()
            << endl;

    auto pathInd = packet->findTag<CpnPathInd>();
    ResponseCandidate candidate;
    candidate.nodeInfo.computeNodeAddress = respInfo->getComputeNodeAddress();
    candidate.nodeInfo.computeNodeId = respInfo->getComputeNodeId();
    candidate.nodeInfo.computeNodePort = respInfo->getComputeNodePort();
    candidate.nodeInfo.computingType = respInfo->getComputingType();
    candidate.nodeInfo.computingCapacity = respInfo->getComputingCapacity();
    candidate.nodeInfo.availableStorage = respInfo->getAvailableStorage();
    candidate.nodeInfo.maxNetworkBandwidth = respInfo->getMaxNetworkBandwidth();
    candidate.nodeInfo.computeCost = respInfo->getComputeCost();
    candidate.nodeInfo.sendTime = respInfo->getSendTime();

    if (pathInd != nullptr) {
        // 网络层记录的 hopAddress 顺序是从上游向用户网关累积的。
        // 应用层在下发 TASK_DATA 时需要从用户网关到算力节点的方向，因此在此反转。
        for (int i = 0; i < pathInd->getHopAddressArraySize(); i++) {
            candidate.sidPath.push_back(pathInd->getHopAddress(i));
        }
        std::reverse(candidate.sidPath.begin(), candidate.sidPath.end());
    }

    if (pathInd != nullptr) {
        EV_INFO << "CPRP route info for task(" << uid << "," << tid << "): "
                << "hopCount=" << pathInd->getHopCount()
                << ", reservedBandwidth=" << pathInd->getReservedBandwidth()
                << " bps, recordedPath(upstreamToUserGateway)=[";
        for (int i = 0; i < pathInd->getHopAddressArraySize(); i++) {
            if (i > 0)
                EV_INFO << " -> ";
            EV_INFO << pathInd->getHopAddress(i);
        }
        EV_INFO << "], sidPath(userGatewayToComputeNode)=[";
        for (size_t i = 0; i < candidate.sidPath.size(); i++) {
            if (i > 0)
                EV_INFO << " -> ";
            EV_INFO << candidate.sidPath[i];
        }
        EV_INFO << "]" << endl;
    }
    else {
        EV_WARN << "CPRP_RESP for task(" << uid << "," << tid
                << ") has no CpnPathInd tag; route information is unavailable." << endl;
    }

    // 当前 RESP 已直接携带网关侧累计时延，应用层不再拆分计算/排队/传输分项时延。
    simtime_t totalDelay = respInfo->getAccumulatedDelay();

    if (candidate.sidPath.empty()) {
        EV_WARN << "Dropping CPRP_RESP candidate for task(" << uid << "," << tid
                << "): route SID path is empty." << endl;
        delete packet;
        return;
    }
    if (candidate.sidPath.back() != respInfo->getComputeNodeAddress()) {
        EV_WARN << "Dropping CPRP_RESP candidate for task(" << uid << "," << tid
                << "): route endpoint " << candidate.sidPath.back()
                << " does not match compute node " << respInfo->getComputeNodeAddress() << "." << endl;
        delete packet;
        return;
    }

    candidate.totalDelay = totalDelay.dbl();
    candidate.reservedBandwidth = respInfo->getRequiredBandwidth();
    candidate.timestamp = simTime();

    candidateCache[{uid, tid}].push_back(candidate);

    EV_INFO << "Recorded path for task (" << uid << "," << tid
            << ") with totalDelay=" << totalDelay
            << " pathHops=" << candidate.sidPath.size() << endl;

    EV_INFO << "Complete CPRP_RESP candidate has been recorded. candidateIndex="
            << (candidateCache[{uid, tid}].size() - 1) << "\n";

    delete packet;
}

void UserGatewayApp::stripCpnPathHeader(Packet *packet)
{
    Ptr<const CpnPathHeader> pathHeader;
    auto frontChunk = packet->peekAtFront<Chunk>(b(-1), OPTIONAL_CPN_PATH_HEADER_PEEK_FLAGS);
    pathHeader = dynamicPtrCast<const CpnPathHeader>(frontChunk);

    if (pathHeader == nullptr) {
        try {
            pathHeader = packet->peekAtFront<CpnPathHeader>(b(-1), OPTIONAL_CPN_PATH_HEADER_TYPED_FLAGS);
        }
        catch (const cRuntimeError& e) {
            EV_WARN << "UserGatewayApp: CpnPathHeader is not present at UDP payload front: " << e.what() << endl;
            return;
        }
    }

    if (pathHeader == nullptr)
        return;

    int mode = pathHeader->getMode();
    if (mode != PATH_RECORD_MODE && mode != PATH_USE_MODE) {
        EV_WARN << "UserGatewayApp: Ignoring invalid CpnPathHeader mode " << mode << endl;
        return;
    }

    auto pathInd = packet->addTagIfAbsent<CpnPathInd>();
    pathInd->setUserId(pathHeader->getUserId());
    pathInd->setTaskId(pathHeader->getTaskId());
    pathInd->setHopAddressArraySize(pathHeader->getHopAddressArraySize());
    for (int i = 0; i < (int)pathHeader->getHopAddressArraySize(); i++)
        pathInd->setHopAddress(i, pathHeader->getHopAddress(i));
    pathInd->setHopCount(pathHeader->getHopAddressArraySize());
    pathInd->setReservedBandwidth(pathHeader->getRequiredBandwidth());

    packet->eraseAtFront(pathHeader->getChunkLength());
    EV_INFO << "UserGatewayApp: Stripped CpnPathHeader from UDP payload and added CpnPathInd tag, hopCount="
            << pathHeader->getHopAddressArraySize() << endl;
}

// 处理算力确认消息
void UserGatewayApp::processCprpConfirm(Packet *packet)
{
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(packet) << std::endl;

    const auto& confirmInfo = packet->popAtFront<CprpConfirmMsg>();

    if(confirmInfo == nullptr){
        EV_WARN << "Error: Received a Packet named '" << packet->getName()
                 << "', but it does not contain a CprpConfirmMsg chunk. Discarding.";
        delete packet;
        return;
    }

    int uid = confirmInfo->getUserId();
    int tid = confirmInfo->getTaskId();
    int selectedNodeId = confirmInfo->getSelectedNodeId();
    L3Address selectedNodeAddress = confirmInfo->getSelectedNodeAddress();
    int selectedNodePort = confirmInfo->getSelectedNodePort();
    int selectedPathIndex = confirmInfo->getSelectedPathIndex();

    EV_INFO << "Processing CPRP_CONFIRM for task (" << uid << "," << tid
            << ") selectedNode=" << selectedNodeId
            << ", selectedNodeAddress=" << selectedNodeAddress
            << ", selectedNodePort=" << selectedNodePort
            << ", selectedPathIndex=" << selectedPathIndex << endl;

    forwardTaskData(uid, tid, selectedNodeId, selectedNodeAddress, selectedNodePort, selectedPathIndex);

    delete packet;
}

void UserGatewayApp::processTaskDataTransferComplete(Packet *packet)
{
    const auto& transferComplete = packet->popAtFront<TaskDataTransferCompleteMsg>();
    if (transferComplete == nullptr) {
        EV_WARN << "Error: Received a Packet named '" << packet->getName()
                << "', but it does not contain a TaskDataTransferCompleteMsg chunk. Discarding." << endl;
        delete packet;
        return;
    }

    int userId = transferComplete->getUserId();
    int taskId = transferComplete->getTaskId();
    auto selectedIt = selectedPathCache.find({userId, taskId});
    if (selectedIt == selectedPathCache.end()) {
        EV_WARN << "TASK_DATA_TRANSFER_COMPLETE for task (" << userId << "," << taskId
                << ") has no selected path cache; cannot revoke route soft state." << endl;
        delete packet;
        return;
    }

    const ResponseCandidate& selectedCandidate = selectedIt->second;
    if (selectedCandidate.nodeInfo.computeNodeAddress != transferComplete->getComputeNodeAddress() ||
        selectedCandidate.nodeInfo.computeNodePort != transferComplete->getComputeNodePort()) {
        EV_WARN << "TASK_DATA_TRANSFER_COMPLETE does not match selected compute node for task (" << userId << "," << taskId
                << "): selected=" << selectedCandidate.nodeInfo.computeNodeAddress << ":" << selectedCandidate.nodeInfo.computeNodePort
                << ", completion=" << transferComplete->getComputeNodeAddress() << ":" << transferComplete->getComputeNodePort() << endl;
        delete packet;
        return;
    }

    sendCancelForCandidate(userId, taskId, selectedCandidate, "TASK_DATA_TRANSFER_COMPLETE");
    selectedPathCache.erase(selectedIt);

    EV_INFO << "Revoked selected CPRP route soft state after TASK_DATA transfer completion for task ("
            << userId << "," << taskId << ")" << endl;
    delete packet;
}

void UserGatewayApp::sendCancelForCandidate(int userId, int taskId, const ResponseCandidate& candidate, const char *reason)
{
    if (candidate.sidPath.empty()) {
        EV_WARN << "Cannot send CPRP_CANCEL for task (" << userId << "," << taskId
                << "): empty SID path, reason=" << reason << endl;
        return;
    }

    std::vector<L3Address> sentAddresses;
    for (const auto& pathNodeAddress : candidate.sidPath) {
        if (pathNodeAddress == candidate.nodeInfo.computeNodeAddress)
            continue;
        if (std::find(sentAddresses.begin(), sentAddresses.end(), pathNodeAddress) != sentAddresses.end())
            continue;
        sentAddresses.push_back(pathNodeAddress);

        auto cancel = makeShared<CancelMsg>();
        cancel->setUserId(userId);
        cancel->setTaskId(taskId);
        cancel->setComputeNodeAddress(candidate.nodeInfo.computeNodeAddress);
        cancel->setComputeNodePort(candidate.nodeInfo.computeNodePort);
        cancel->setSenderType(SENDER_USER_GW);

        Packet *cancelPacket = new Packet("CPRP_CANCEL");
        cancelPacket->insertAtBack(cancel);
        socket.sendTo(cancelPacket, pathNodeAddress, computeGatewayPort);

        EV_INFO << "Sent CPRP_CANCEL for task=(" << userId << "," << taskId
                << "), computeNode=" << candidate.nodeInfo.computeNodeId << "@" << candidate.nodeInfo.computeNodeAddress
                << ":" << candidate.nodeInfo.computeNodePort
                << ", pathNode=" << pathNodeAddress
                << ", reason=" << reason << endl;
    }
}

void UserGatewayApp::cancelUnselectedCandidates(int userId, int taskId, int selectedPathIndex, const std::vector<ResponseCandidate>& candidates)
{
    for (int i = 0; i < (int)candidates.size(); i++) {
        if (i == selectedPathIndex)
            continue;

        const ResponseCandidate& candidate = candidates[i];
        if (candidate.sidPath.empty()) {
            EV_WARN << "Cannot send CPRP_CANCEL for unselected candidate: empty SID path, task=(" << userId << "," << taskId
                    << "), candidateIndex=" << i << endl;
            continue;
        }

        sendCancelForCandidate(userId, taskId, candidate, "UNSELECTED_CANDIDATE");
    }
}

void UserGatewayApp::forwardTaskData(int userId, int taskId, int selectedNodeId, const L3Address& selectedNodeAddress, int selectedNodePort, int selectedPathIndex)
{
    auto candidateIt = candidateCache.find({userId, taskId});
    if (candidateIt == candidateCache.end() || candidateIt->second.empty()) {
        EV_ERROR << "No response candidate found for task (" << userId << "," << taskId << ")" << endl;
        return;
    }

    auto requestIt = requestContextCache.find({userId, taskId});
    if (requestIt == requestContextCache.end()) {
        EV_ERROR << "No request context found for task (" << userId << "," << taskId << ")" << endl;
        return;
    }

    std::vector<ResponseCandidate>* candidates = &candidateIt->second;
    const RequestContext& requestContext = requestIt->second;

    if (selectedPathIndex < 0 || selectedPathIndex >= (int)candidates->size()) {
        EV_ERROR << "Selected path index " << selectedPathIndex << " is out of range for task ("
                 << userId << "," << taskId << "), candidateCount=" << candidates->size() << endl;
        return;
    }

    ResponseCandidate* selectedCandidate = &candidates->at(selectedPathIndex);
    if (selectedCandidate->nodeInfo.computeNodeId != selectedNodeId || selectedCandidate->nodeInfo.computeNodeAddress != selectedNodeAddress || selectedCandidate->nodeInfo.computeNodePort != selectedNodePort) {
        EV_ERROR << "Selected path does not match CPRP_CONFIRM node for task (" << userId << "," << taskId
                 << "): candidateNode=" << selectedCandidate->nodeInfo.computeNodeId << "@" << selectedCandidate->nodeInfo.computeNodeAddress
                 << ":" << selectedCandidate->nodeInfo.computeNodePort
                 << ", confirmNode=" << selectedNodeId << "@" << selectedNodeAddress << ":" << selectedNodePort << endl;
        return;
    }

    if (selectedCandidate->sidPath.empty()) {
        EV_ERROR << "Selected candidate path is empty" << endl;
        return;
    }

    cancelUnselectedCandidates(userId, taskId, selectedPathIndex, *candidates);
    selectedPathCache[{userId, taskId}] = *selectedCandidate;

    EV_INFO << "Selected cached CPRP path index=" << selectedPathIndex
            << " delay=" << selectedCandidate->totalDelay
            << " hops=" << selectedCandidate->sidPath.size() << endl;

    auto taskData = makeShared<TaskDataMsg>();
    taskData->setUserId(userId);
    taskData->setTaskId(taskId);
    taskData->setUserNodeAddress(requestContext.userNodeAddress);
    taskData->setUserNodePort(requestContext.userNodePort);
    taskData->setGenerationTime(requestContext.generationTime);
    taskData->setComputingType(requestContext.computingType);
    taskData->setRequiredStorage(requestContext.requiredStorage);
    taskData->setComputingAmount(requestContext.computingAmount);
    taskData->setTransferAmount(requestContext.transferAmount);
    taskData->setTotalDelayRequirement(requestContext.totalDelayRequirement);
    taskData->setBudget(requestContext.budget);
    taskData->setUserMaxBandwidth(requestContext.userMaxBandwidth);
    taskData->setPriority(5);

    Packet *pkt = new Packet("TASK_DATA");
    pkt->insertAtBack(taskData);

    auto pathReq = pkt->addTagIfAbsent<CpnPathReq>();
    pathReq->setMode(PATH_USE_MODE);
    pathReq->setUserId(userId);
    pathReq->setTaskId(taskId);
    pathReq->setSidListArraySize(selectedCandidate->sidPath.size());
    for (size_t i = 0; i < selectedCandidate->sidPath.size(); i++) {
        pathReq->setSidList(i, selectedCandidate->sidPath[i]);
    }
    pathReq->setCurrentHopIndex(0);

    pkt->addTagIfAbsent<DscpReq>()->setDifferentiatedServicesCodePoint(5);

    L3Address firstHop = selectedCandidate->sidPath[0];
    // 数据包通过源路由逐跳转发到目标算力节点，因此这里的 UDP 目的端口必须与目标节点监听端口一致。
    socket.sendTo(pkt, firstHop, selectedNodePort > 0 ? selectedNodePort : selectedCandidate->nodeInfo.computeNodePort);

    EV_INFO << "Forwarding TASK_DATA via selected path to " << firstHop << endl;

    candidateCache.erase({userId, taskId});
    requestContextCache.erase({userId, taskId});
}


// UdpSocket::ICallback
void UserGatewayApp::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    if(strcmp(packet->getName(), "TaskRequestMsg") == 0){
        sendCprpRequest(packet);
    }
    else if(strcmp(packet->getName(), "CPRP_RESP") == 0){
        processCprpResp(packet);
    }
    else if(strcmp(packet->getName(), "CPRP_CONFIRM") == 0){
        processCprpConfirm(packet);
    }
    else if(strcmp(packet->getName(), "CPRP_CANCEL") == 0){
        // 用户网关应用层在这里接收来自算力网关的撤销/完成通知。
        // 当前回滚状态下暂不维护额外软状态，因此先记录日志并释放报文。
        EV_INFO << "UserGatewayApp received CPRP_CANCEL." << endl;
        delete packet;
    }
    else if(strcmp(packet->getName(), "TASK_DATA_TRANSFER_COMPLETE") == 0){
        processTaskDataTransferComplete(packet);
    }
    else{
        EV_WARN << "Unknown packet type: " << packet->getName() << endl;
        delete packet;
    }
}

void UserGatewayApp::socketErrorArrived(UdpSocket *socket, Indication *indication)
{

}

void UserGatewayApp::socketClosed(UdpSocket *socket)
{

}

void UserGatewayApp::finish()
{

}

void UserGatewayApp::handleStartOperation(LifecycleOperation *operation)
{
//    socket.setOutputGate(gate("socketOut"));
//    socket.bind(localAddress, localPort);
}

void UserGatewayApp::handleStopOperation(LifecycleOperation *operation)
{
    socket.close();
}

void UserGatewayApp::handleCrashOperation(LifecycleOperation *operation)
{
    socket.destroy();
}

} /* namespace inet */
