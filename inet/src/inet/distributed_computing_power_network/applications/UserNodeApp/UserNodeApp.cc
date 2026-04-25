

#include "inet/networklayer/common/L3AddressResolver.h"
#include <string>
#include "UserNodeApp.h"

Define_Module(inet::UserNodeApp);

namespace inet {

UserNodeApp::UserNodeApp() {
    // TODO Auto-generated constructor stub

}

UserNodeApp::~UserNodeApp() {
    // TODO Auto-generated destructor stub
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
    EV_INFO << "User" << userNodeId << " is to send TaskRequestMsg...\n";

    auto payload = makeShared<TaskRequestMsg>();
    payload->setUserId(this->userNodeId);
    payload->setTaskId(1);  // test，暂时硬编码
    payload->setComputingType(0);   // test，暂时硬编码
    payload->setUserMaxBandwidth(this->maxTransmissionBandwidth);

    std::string messageType = payload->getMsgType();
    Packet *pkt = new Packet(messageType.c_str());
    pkt->insertAtBack(payload);

    socket.sendTo(pkt, userGatewayAddress, userGatewayPort);

    EV_INFO << "User" << this->userNodeId << " has sent TaskRequestMsg for task(" << this->userNodeId << "-" << payload->getTaskId() << ")\n";
}

void UserNodeApp::sendCprpConfirm(Packet *packet)
{
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(packet) << std::endl;

    // 提取任务请求负载信息
    const auto& sumInfo = packet->popAtFront<RespSummaryMsg>();

    if(sumInfo == nullptr){
        EV_WARN << "Error: Received a Packet named '" << packet->getName()
                 << "', but it does not contain a RespSummaryMsg chunk. Discarding.";
        delete packet;
        return;
    }

    int selectedIndex = 0;
    if (sumInfo->getNodeInfoArraySize() == 0) {
        EV_WARN << "No compute node info available" << std::endl;
        delete packet;
        return;
    }
    
    computeNodeInfo selectedNode = sumInfo->getNodeInfo(selectedIndex);

    auto payload = makeShared<CprpConfirmMsg>();
    payload->setUserId(sumInfo->getUserId());
    payload->setTaskId(sumInfo->getTaskId());
    payload->setSelectedNodeId(selectedNode.computeNodeId);
    payload->setSelectedNodeAddress(selectedNode.computeNodeAddress);
    payload->setSelectedNodePort(5000);

    std::string messageType = payload->getMsgType();
    Packet *pkt = new Packet(messageType.c_str());
    pkt->insertAtBack(payload);

    socket.sendTo(pkt, userGatewayAddress, userGatewayPort);

    EV_INFO << "User sent CPRP_CONFIRM for task (" << sumInfo->getUserId() << "," << sumInfo->getTaskId()
            << ") selectedNode=" << selectedNode.computeNodeAddress << std::endl;

    delete packet;
}

// UdpSocket::ICallback
void UserNodeApp::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    if(strcmp(packet->getName(), "RespSummaryMsg") == 0){
        sendCprpConfirm(packet);
    }
    else if(strcmp(packet->getName(), "") == 0){

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
