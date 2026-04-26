
#include <string>
#include <sstream>
#include <algorithm>
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/InterfaceTable.h"
#include "inet/networklayer/common/DscpTag_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/distributed_computing_power_network/message/CpnPathHeader_m.h"
#include "UserGatewayApp.h"

Define_Module(inet::UserGatewayApp);

namespace inet {

UserGatewayApp::UserGatewayApp() {
    // TODO Auto-generated constructor stub

}

UserGatewayApp::~UserGatewayApp() {
    // TODO Auto-generated destructor stub
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

            EV_INFO << "Received RespTimeoutSelfMsg. To Send collected computeNode info to userNode for task(" << uid << "-" << tid << ")\n";
            // 发送可用算力信息至对应用户节点
            sendCollectedNodeInfo(uid,tid);
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

    // 提取任务请求负载信息
    const auto& requestInfo = packet->popAtFront<TaskRequestMsg>();

    if(requestInfo == nullptr){
        EV_WARN << "Error: Received a Packet named '" << packet->getName()
                 << "', but it does not contain a TaskRequestMsg chunk. Discarding.";
        delete packet;
        return;
    }

    // 创建算力请求载荷
    auto payload = makeShared<CprpRequestMsg>();
    payload->setUserId(requestInfo->getUserId());
    payload->setTaskId(requestInfo->getTaskId());
    payload->setUserGatewayAddress(localAddress);
    payload->setGenerationTime(requestInfo->getGenerationTime());
    payload->setComputingType(requestInfo->getComputingType());
    payload->setRequiredStorage(requestInfo->getRequiredStorage());
    payload->setComputingAmount(requestInfo->getComputingAmount());
    payload->setTransferAmount(requestInfo->getTransferAmount());
    payload->setTotalDelayRequirement(requestInfo->getTotalDelayRequirement());
    payload->setBudget(requestInfo->getBudget());

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

    // 启动算力请求计时
    startCprpRequestTimer(payload->getUserId(),payload->getTaskId());

    // 清理任务请求消息
    delete packet;
}

void UserGatewayApp::sendCollectedNodeInfo(int uid, int tid)
{
    EV_INFO << "Start sending collected computeNode info to userNode" << uid << " for task(" << uid << "-" << tid << ")\n";

    auto& cpArray = cpMap.at({uid,tid});

    auto payload = makeShared<RespSummaryMsg>();
    payload->setNodeInfoArraySize(cpArray.size());
    for(auto nodeInfo:cpArray){
        for(int i=0;i<payload->getNodeInfoArraySize();i++){
            payload->setNodeInfo(i, nodeInfo);
        }
    }

    payload->setUserId(uid);
    payload->setTaskId(tid);

    std::string messageType = payload->getMsgType();
    Packet *pkt = new Packet(messageType.c_str());
    pkt->insertAtBack(payload);

    L3Address userAddress = userNodeIpMap.at(uid);
    socket.sendTo(pkt, userAddress, userNodePort);

    EV_INFO << "UserGateway" << userGatewayId << " has sent RespSummaryMsg to user" << uid << std::endl;
}

// 启动算力请求计时器
void UserGatewayApp::startCprpRequestTimer(int userId, int taskId)
{
    RespTimeoutSelfMsg *timer = new RespTimeoutSelfMsg("RespTimeoutSelfMsg");

    timer->setUserId(userId);
    timer->setTaskId(taskId);

    // 时限设置
    scheduleAt(simTime() + 5, timer);
}

// 算力应答消息处理
void UserGatewayApp::processCprpResp(Packet *packet)
{
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(packet) << std::endl;

    const auto& respInfo = packet->popAtFront<CprpResponseMsg>();

    if(respInfo == nullptr){
        EV_WARN << "Error: Received a Packet named '" << packet->getName()
                 << "', but it does not contain a CprpResponseMsg chunk. Discarding.";
        delete packet;
        return;
    }

    int uid = respInfo->getUserId();
    int tid = respInfo->getTaskId();
    L3Address computeNodeAddr = respInfo->getComputeNodeAddress();
    int computeNodePort = respInfo->getComputeNodePort();
    int computeNodeId = respInfo->getComputeNodeId();
    
    EV_INFO << "Processing CPRP_RESP for task (" << uid << "," << tid 
            << ") computeNode" << computeNodeId << " = " << computeNodeAddr << ":" << computeNodePort << std::endl;

    if (isTimerExpired(uid, tid)) {
        EV_INFO << "Timer expired for task (" << uid << "," << tid 
                << "), sending CANCEL" << std::endl;
        
        auto pathInd = packet->findTag<CpnPathInd>();
        if (pathInd) {
            std::vector<L3Address> pathNodes;
            for (int i = 0; i < pathInd->getHopAddressArraySize(); i++) {
                pathNodes.push_back(pathInd->getHopAddress(i));
            }
            sendCancelToPathNodes(uid, tid, computeNodeAddr, computeNodePort, pathNodes);
        }
        delete packet;
        return;
    }

    auto& cpArray = cpMap[{uid,tid}];
    computeNodeInfo cpNodeInfo;
    cpNodeInfo.computeNodeAddress = computeNodeAddr;
    cpNodeInfo.computeNodeId = respInfo->getComputeNodeId();
    cpNodeInfo.computingType = respInfo->getComputingType();
    cpNodeInfo.computingCapacity = respInfo->getComputingCapacity();
    cpNodeInfo.availableStorage = respInfo->getAvailableStorage();
    cpNodeInfo.sendTime = respInfo->getSendTime();
    cpArray.push_back(cpNodeInfo);

    auto pathInd = packet->findTag<CpnPathInd>();
    PathInfo pathInfo;
    
    if (pathInd != nullptr) {
        for (int i = 0; i < pathInd->getHopAddressArraySize(); i++) {
            pathInfo.sidPath.push_back(pathInd->getHopAddress(i));
        }
        std::reverse(pathInfo.sidPath.begin(), pathInfo.sidPath.end());
    }
    
    pathInfo.computeNodeAddress = computeNodeAddr;
    pathInfo.computeNodePort = computeNodePort;
    pathInfo.computeNodeId = respInfo->getComputeNodeId();
    pathInfo.totalDelay = respInfo->getAccumulatedDelay().dbl();
    pathInfo.computeCost = respInfo->getComputeCost();
    pathInfo.bandwidth = respInfo->getRequiredBandwidth();
    pathInfo.timestamp = simTime();
    
    std::string key = computeNodeAddr.str() + ":" + std::to_string(computeNodePort);
    pathCache[{uid, tid}][key] = pathInfo;

    EV_INFO << "Recorded session for task (" << uid << "," << tid 
            << ") computeNode" << pathInfo.computeNodeId << " = " << computeNodeAddr << ":" << computeNodePort
            << " with totalDelay=" << pathInfo.totalDelay << std::endl;

    delete packet;
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
    L3Address selectedNodeAddr = confirmInfo->getSelectedNodeAddress();
    int selectedNodePort = confirmInfo->getSelectedNodePort();
    int computingType = confirmInfo->getComputingType();

    EV_INFO << "Processing CPRP_CONFIRM for task (" << uid << "," << tid 
            << ") selectedNode=" << selectedNodeAddr << ":" << selectedNodePort << std::endl;

    auto taskIt = pathCache.find({uid, tid});
    if (taskIt == pathCache.end()) {
        EV_ERROR << "No sessions found for task (" << uid << "," << tid << ")" << std::endl;
        delete packet;
        return;
    }
    
    std::string selectedKey = selectedNodeAddr.str() + ":" + std::to_string(selectedNodePort);
    
    for (const auto& pair : taskIt->second) {
        if (pair.first != selectedKey) {
            EV_INFO << "Sending CANCEL for unselected session: " << pair.first << std::endl;
            sendCancelToPathNodes(uid, tid, 
                                  pair.second.computeNodeAddress,
                                  pair.second.computeNodePort,
                                  pair.second.sidPath);
        }
    }
    
    PathInfo selectedPath = taskIt->second[selectedKey];
    pathCache.erase({uid, tid});
    pathCache[{uid, tid}][selectedKey] = selectedPath;

    forwardTaskData(uid, tid, selectedNodeId, computingType);

    delete packet;
}

// 转发任务数据消息
void UserGatewayApp::forwardTaskData(int userId, int taskId, int selectedNodeId, int computingType)
{
    auto it = pathCache.find({userId, taskId});
    if (it == pathCache.end() || it->second.empty()) {
        EV_ERROR << "No path found for task (" << userId << "," << taskId << ")" << endl;
        return;
    }

    PathInfo* selectedPath = nullptr;
    
    for (auto& pair : it->second) {
        if (pair.second.computeNodeId == selectedNodeId) {
            selectedPath = &pair.second;
            break;
        }
    }

    if (!selectedPath || selectedPath->sidPath.empty()) {
        EV_ERROR << "Selected node path not found or path is empty" << endl;
        return;
    }

    EV_INFO << "Selected path with delay=" << selectedPath->totalDelay 
            << " hops=" << selectedPath->sidPath.size() << endl;

    auto taskData = makeShared<TaskDataMsg>();
    taskData->setUserId(userId);
    taskData->setTaskId(taskId);
    taskData->setComputingType(computingType);
    taskData->setPriority(5);

    Packet *pkt = new Packet("TASK_DATA");
    pkt->insertAtBack(taskData);

    auto pathReq = pkt->addTagIfAbsent<CpnPathReq>();
    pathReq->setMode(PATH_USE_MODE);
    pathReq->setUserId(userId);
    pathReq->setTaskId(taskId);
    pathReq->setSidListArraySize(selectedPath->sidPath.size());
    for (size_t i = 0; i < selectedPath->sidPath.size(); i++) {
        pathReq->setSidList(i, selectedPath->sidPath[i]);
    }
    pathReq->setCurrentHopIndex(0);

    pkt->addTagIfAbsent<DscpReq>()->setDifferentiatedServicesCodePoint(5);

    L3Address firstHop = selectedPath->sidPath[0];
    socket.sendTo(pkt, firstHop, computeNodePort);

    EV_INFO << "Forwarding TASK_DATA via selected path to " << firstHop << endl;

    pathCache.erase({userId, taskId});
}

// 处理CANCEL消息
void UserGatewayApp::processCancelMsg(Packet *packet)
{
    EV_INFO << "Received CANCEL packet: " << UdpSocket::getReceivedPacketInfo(packet) << std::endl;
    
    const auto& cancel = packet->peekAtFront<CancelMsg>();
    
    if (cancel == nullptr) {
        EV_WARN << "Invalid CancelMsg" << std::endl;
        delete packet;
        return;
    }
    
    int uid = cancel->getUserId();
    int tid = cancel->getTaskId();
    L3Address computeNodeAddr = cancel->getComputeNodeAddress();
    int computeNodePort = cancel->getComputeNodePort();
    
    EV_INFO << "Processing CANCEL for task (" << uid << "," << tid 
            << ") computeNode=" << computeNodeAddr << ":" << computeNodePort << std::endl;
    
    std::string key = computeNodeAddr.str() + ":" + std::to_string(computeNodePort);
    auto taskIt = pathCache.find({uid, tid});
    if (taskIt == pathCache.end()) {
        EV_WARN << "No sessions found for task (" << uid << "," << tid << ")" << std::endl;
        delete packet;
        return;
    }
    
    auto pathIt = taskIt->second.find(key);
    if (pathIt == taskIt->second.end()) {
        EV_WARN << "Session not found: " << key << std::endl;
        delete packet;
        return;
    }
    
    sendCancelToPathNodes(uid, tid, computeNodeAddr, computeNodePort, pathIt->second.sidPath);
    
    taskIt->second.erase(key);
    EV_INFO << "Removed session: " << key << std::endl;
    
    delete packet;
}

// 检查计时器是否过期
bool UserGatewayApp::isTimerExpired(int userId, int taskId)
{
    auto it = requestTimers.find({userId, taskId});
    if (it == requestTimers.end()) return false;
    return simTime() > it->second;
}

// 向路径节点发送CANCEL
void UserGatewayApp::sendCancelToPathNodes(int userId, int taskId,
                                            const L3Address& computeNodeAddr, int computeNodePort,
                                            const std::vector<L3Address>& path)
{
    EV_INFO << "Sending CANCEL to " << path.size() << " path nodes for task (" 
            << userId << "," << taskId << ") computeNode=" << computeNodeAddr 
            << ":" << computeNodePort << std::endl;
    
    for (const auto& nodeAddr : path) {
        auto cancel = makeShared<CancelMsg>();
        cancel->setUserId(userId);
        cancel->setTaskId(taskId);
        cancel->setComputeNodeAddress(computeNodeAddr);
        cancel->setComputeNodePort(computeNodePort);
        cancel->setSenderType(SENDER_USER_GW);
        
        Packet *pkt = new Packet("CANCEL");
        pkt->insertAtBack(cancel);
        
        socket.sendTo(pkt, nodeAddr, computeGatewayPort);
        
        EV_INFO << "Sent CANCEL to " << nodeAddr << std::endl;
    }
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
    else if(strcmp(packet->getName(), "CANCEL") == 0){
        processCancelMsg(packet);
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
