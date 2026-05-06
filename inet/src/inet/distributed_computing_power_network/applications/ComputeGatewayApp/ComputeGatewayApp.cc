
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include <string>
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

//        // 加入全局算力请求组
//        L3Address groupAddress = L3AddressResolver().resolve(par("groupAddress"));
//        socket.joinMulticastGroup(groupAddress);
//
//        EV_INFO << "Joined group: " << groupAddress << endl;

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
        auto payload = makeShared<CgmpQueryMsg>();

        std::string messageType = payload->getMsgType();
        Packet *pkt = new Packet(messageType.c_str());
        pkt->insertAtBack(payload);

        socket.sendTo(pkt, cpGroupAddress, computeNodePort);

        EV_INFO << "CGMP_Query has been sent\n";
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

    EV_INFO << "CIB has been updated: computingType:" << cib.computingType << std::endl;

    delete packet;
}

// 发送算力应答
void ComputeGatewayApp::sendCprpResponse(Packet *packet)
{
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(packet) << std::endl;

    // 提取算力请求负载信息
    const auto& requestInfo = packet->popAtFront<CprpRequestMsg>();

    if(requestInfo == nullptr){
        EV_WARN << "Error: Received a Packet named '" << packet->getName()
                 << "', but it does not contain a CprpRequestMsg chunk. Discarding.";
        delete packet;
        return;
    }

    // 算力组查询
    if(cibInfoMap.find(requestInfo->getComputingType()) == cibInfoMap.end()){
        EV_INFO << "ComputeGateway" << computeGatewayId << " has no cib entry for computingType: " << requestInfo->getComputingType() << std::endl;
        return;
    }

    const auto& groupMap = cibInfoMap.at(requestInfo->getComputingType());

    // 算力节点选择算法
    int selectedNodeId = 1; // test，暂时硬编码

    CIB destNodeInfo;
    if(groupMap.find(selectedNodeId)==groupMap.end())
        destNodeInfo = groupMap.at(4);   // test，暂时硬编码
    else
        destNodeInfo = groupMap.at(selectedNodeId);   // test，暂时硬编码


    // 创建算力应答载荷
    auto payload = makeShared<CprpResponseMsg>();
    payload->setUserId(requestInfo->getUserId());
    payload->setTaskId(requestInfo->getTaskId());
    payload->setComputeNodeId(selectedNodeId);

    payload->setComputeNodeAddress(destNodeInfo.nodeAddress);
    payload->setComputingType(destNodeInfo.computingType);
    payload->setComputingCapacity(destNodeInfo.computingCapacity);
    payload->setAvailableStorage(destNodeInfo.availableStorage);
    payload->setSendTime(simTime());

    // 创建并转发Packet至源用户网关
    std::string messageType = payload->getMsgType();
    Packet *pkt = new Packet(messageType.c_str());
    pkt->insertAtBack(payload);

    socket.sendTo(pkt, requestInfo->getUserGatewayAddress(), userGatewayPort);

    EV_INFO << "ComputeGateway" << computeGatewayId << " has sent CPRP_RESP.\n";

    // 清理算力请求消息
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

} /* namespace inet */
