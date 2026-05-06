#include<cstring>
#include "ComputeNodeApp.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/HopLimitTag_m.h"


Define_Module(inet::ComputeNodeApp);

namespace inet {

ComputeNodeApp::ComputeNodeApp() {
    // TODO Auto-generated constructor stub

}

ComputeNodeApp::~ComputeNodeApp() {
    // TODO Auto-generated destructor stub
}

void ComputeNodeApp::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
       this->computeNodeId = par("computeNodeId");
       this->computeGatewayId = par("computeGatewayId");
       this->localPort = par("localPort");
       this->localAddress = L3AddressResolver().resolve(par("localAddress"));
       this->computeGatewayAddress = L3AddressResolver().resolve(par("computeGatewayAddress"));
       this->computeGatewayPort = par("computeGatewayPort");
       this->computingType = par("computingType");
       this->computingCapacity = par("computingCapacity");
       this->storageCapacity = par("storageCapacity");
       // availableStorage 表示当前可用容量。初始化时应与 storageCapacity 一致，
       // 后续若引入任务执行逻辑，再由任务生命周期动态扣减与释放。
       this->availableStorage = this->storageCapacity;
       this->multicastAddress = L3AddressResolver().resolve(par("multicastAddress"));
    }

    else if (stage == INITSTAGE_APPLICATION_LAYER) {
//        scheduleAt(simTime(), selfTaskCreationEvent);
        socket.setOutputGate(gate("socketOut"));
        socket.bind(localAddress, localPort);
        socket.setCallback(this);   // 将当前应用实例注册为 socket 的回调处理对象
        socket.setMulticastLoop(false);

        // 加入算力组。
        // 当前回滚状态下，ComputeNodeApp 仅承担算力状态上报职责，
        // 尚未恢复 TASK_DATA / TASK_COMPLETION 的完整执行链路。
        socket.joinMulticastGroup(multicastAddress);

        EV_INFO << "ComputeNode" << computeNodeId << " initialized and joined multicast group: " << multicastAddress << endl;
    }

}

// 发送组成员报告
void ComputeNodeApp::sendCgmpReport()
{
    EV_INFO << "Start sending CGMP_Report...\n";

    auto payload = makeShared<CgmpReportMsg>();
    payload->setComputeNodeId(computeNodeId);
    payload->setComputingType(computingType);
    payload->setComputeNodeAddress(localAddress);
    payload->setComputingCapacity(computingCapacity);
    payload->setAvailableStorage(availableStorage);
    payload->setSendTime(simTime());

    std::string messageType = payload->getMsgType();
    Packet *pkt = new Packet(messageType.c_str());
    pkt->insertAtBack(payload);

    // CGMP_Report 只应在本地接入域内传播，不应穿越上游算力路由器，
    // 因此显式限制 HopLimit 为 1。
    auto hopLimitReq = pkt->addTagIfAbsent<HopLimitReq>();
    hopLimitReq->setHopLimit(1);

    socket.sendTo(pkt, multicastAddress, computeGatewayPort);

    EV_INFO << "ComputeNode" << computeNodeId << " has sent CGMP_Report (TTL=1) to computeGateway" << computeGatewayId << "\n";
}


void ComputeNodeApp::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {

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
        sendCgmpReport();
        delete packet;
    }
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
