
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
    cib.maxNetworkBandwidth = reportInfo->getMaxNetworkBandwidth();
    cib.computeCost = reportInfo->getComputeCost();
    cib.updateTime = reportInfo->getSendTime();

    EV_INFO << "CIB has been updated: computingType:" << cib.computingType << std::endl;

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

    int selectedNodeId = 1; // TODO: replace with a real selection algorithm

    CIB destNodeInfo;
    auto selectedIt = groupMap.find(selectedNodeId);
    if (selectedIt != groupMap.end())
        destNodeInfo = selectedIt->second;
    else
        destNodeInfo = groupMap.begin()->second;

    auto payload = makeShared<CprpResponseMsg>();
    payload->setUserId(requestInfo->getUserId());
    payload->setTaskId(requestInfo->getTaskId());
    payload->setComputeNodeId(destNodeInfo.nodeId);

    payload->setComputeNodeAddress(destNodeInfo.nodeAddress);
    payload->setComputeNodePort(computeNodePort);
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
    // 网关侧尚未引入更细粒度的固定时延模型，先以 0 作为累计时延初值，
    // 后续由沿途网络层节点继续叠加路径相关时延。
    payload->setAccumulatedDelay(SIMTIME_ZERO);

    std::string messageType = payload->getMsgType();
    Packet *pkt = new Packet(messageType.c_str());
    pkt->insertAtBack(payload);

    auto pathReq = pkt->addTagIfAbsent<CpnPathReq>();
    pathReq->setMode(PATH_RECORD_MODE);
    pathReq->setUserId(requestInfo->getUserId());
    pathReq->setTaskId(requestInfo->getTaskId());
    pathReq->setUserGatewayAddress(requestInfo->getUserGatewayAddress());
    pathReq->setRequiredBandwidth(requestInfo->getUserMaxBandwidth() * 1e6);
    // 应用层预先写入上游路径起点：[算力节点, 算力网关]。
    // 后续算力路由器在网络层继续向 hopAddress 末尾追加自身出口地址。
    pathReq->setHopAddressArraySize(2);
    pathReq->setHopAddress(0, destNodeInfo.nodeAddress);
    pathReq->setHopAddress(1, localAddress);

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
        // 网络层已先完成带宽/会话撤销。应用层这里只预留软状态清理入口。
        EV_INFO << "ComputeGatewayApp received CPRP_CANCEL." << std::endl;
        delete packet;
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
