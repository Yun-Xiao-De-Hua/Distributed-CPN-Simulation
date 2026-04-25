
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/InterfaceTable.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/distributed_computing_power_network/message/CpnPathHeader_m.h"
#include <string>
#include <sstream>
#include "ComputeGatewayApp.h"

Define_Module(inet::ComputeGatewayApp);

namespace inet {

ComputeGatewayApp::ComputeGatewayApp() {
    // TODO Auto-generated constructor stub

}

ComputeGatewayApp::~ComputeGatewayApp() {
    // TODO Auto-generated destructor stub
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
        // 处理底层传上来的 UDP 数据
        socket.processMessage(msg);
    }
}

// 发送组成员查询消息
void ComputeGatewayApp::ComputeGatewayApp::sendCgmpQuery()
{
    EV_INFO << "Start Sending CGMP_Query for CIB updating..." << std::endl;

    for(const auto& cpGroupAddress : computingPowerGroup){
        auto routeIt = multicastRoutesMap.find(cpGroupAddress);
        
        if (routeIt == multicastRoutesMap.end() || routeIt->second.empty()) {
            EV_WARN << "No forwarding interfaces configured for multicast group " << cpGroupAddress 
                    << ", using default sendTo" << endl;
            auto payload = makeShared<CgmpQueryMsg>();
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

    // 算力组 CIB 更新
    auto& groupMap = cibInfoMap[reportInfo->getComputingType()];
    CIB& cib = groupMap[reportInfo->getComputeNodeId()];
    cib.nodeId = reportInfo->getComputeNodeId();
    cib.computingType = reportInfo->getComputingType();
    cib.nodeAddress = reportInfo->getComputeNodeAddress();
    cib.computingCapacity = reportInfo->getComputingCapacity();
    cib.availableStorage = reportInfo->getAvailableStorage();
    cib.updateTime = reportInfo->getSendTime();

    EV_INFO << "CIB has been updated: computingType:" << cib.computingType << std::endl;

    delete packet;
}

// 发送算力应答
void ComputeGatewayApp::sendCprpResponse(Packet *packet)
{
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(packet) << std::endl;

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

    int selectedNodeId = 1;

    CIB destNodeInfo;
    if(groupMap.find(selectedNodeId)==groupMap.end())
        destNodeInfo = groupMap.at(4);
    else
        destNodeInfo = groupMap.at(selectedNodeId);

    auto payload = makeShared<CprpResponseMsg>();
    payload->setUserId(requestInfo->getUserId());
    payload->setTaskId(requestInfo->getTaskId());
    payload->setComputeNodeId(selectedNodeId);
    payload->setComputeNodeAddress(destNodeInfo.nodeAddress);
    payload->setComputeNodePort(computeNodePort);

    payload->setComputingType(destNodeInfo.computingType);
    payload->setComputingCapacity(destNodeInfo.computingCapacity);
    payload->setAvailableStorage(destNodeInfo.availableStorage);
    payload->setSendTime(simTime());

    payload->setRequiredBandwidth(requestInfo->getUserMaxBandwidth());
    payload->setMaxDelayTolerance(requestInfo->getTotalDelayRequirement());
    payload->setComputeCost(10.0);
    
    double computingDelay = 0.001;
    double queuingDelay = 0.0005;
    double transmissionDelay = 0.001;
    double totalDelay = computingDelay + queuingDelay + transmissionDelay;
    
    payload->setAccumulatedDelay(totalDelay);
    
    payload->setLastHopSendTime(simTime());
    payload->setLastHopAddress(localAddress);

    std::string messageType = payload->getMsgType();
    Packet *pkt = new Packet(messageType.c_str());
    pkt->insertAtBack(payload);

    auto pathReq = pkt->addTagIfAbsent<CpnPathReq>();
    pathReq->setMode(PATH_RECORD_MODE);
    pathReq->setUserId(requestInfo->getUserId());
    pathReq->setTaskId(requestInfo->getTaskId());
    pathReq->setUserGatewayAddress(requestInfo->getUserGatewayAddress());
    pathReq->setRequiredBandwidth(requestInfo->getUserMaxBandwidth());
    
    pathReq->setHopAddressArraySize(2);
    pathReq->setHopAddress(0, destNodeInfo.nodeAddress);
    pathReq->setHopAddress(1, localAddress);

    socket.sendTo(pkt, requestInfo->getUserGatewayAddress(), userGatewayPort);

    EV_INFO << "ComputeGateway" << computeGatewayId << " has sent CPRP_RESP for task(" 
            << requestInfo->getUserId() << "," << requestInfo->getTaskId() 
            << ") with computeNode=" << destNodeInfo.nodeAddress 
            << ":" << computeNodePort << ".\n";

    delete packet;
}

// 处理任务完成通告
void ComputeGatewayApp::handleTaskCompletion(Packet *packet)
{
    EV_INFO << "Received task completion notification" << std::endl;
    
    const auto& completion = packet->peekAtFront<TaskCompletionMsg>();
    
    if (completion == nullptr) {
        EV_WARN << "Invalid TaskCompletionMsg" << std::endl;
        delete packet;
        return;
    }
    
    int uid = completion->getUserId();
    int tid = completion->getTaskId();
    L3Address computeNodeAddr = completion->getComputeNodeAddress();
    int cnPort = completion->getComputeNodePort();
    L3Address userGwAddr = completion->getUserGatewayAddress();
    
    EV_INFO << "Task (" << uid << "," << tid << ") completed by computeNode " 
            << computeNodeAddr << ":" << cnPort << std::endl;
    
    auto cancel = makeShared<CancelMsg>();
    cancel->setUserId(uid);
    cancel->setTaskId(tid);
    cancel->setComputeNodeAddress(computeNodeAddr);
    cancel->setComputeNodePort(cnPort);
    cancel->setSenderType(SENDER_COMPUTE_GW);
    
    Packet *cancelPkt = new Packet("CANCEL");
    cancelPkt->insertAtBack(cancel);
    
    socket.sendTo(cancelPkt, userGwAddr, userGatewayPort);
    
    EV_INFO << "Sent CANCEL to user gateway " << userGwAddr << " for task (" 
            << uid << "," << tid << ")" << std::endl;
    
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
    else if(strcmp(packet->getName(), "TASK_COMPLETION") == 0){
        handleTaskCompletion(packet);
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
