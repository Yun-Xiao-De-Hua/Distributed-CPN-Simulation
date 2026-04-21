
#include <string>
#include <sstream>
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/InterfaceTable.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
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
        socket.bind(localPort); // 监听本机所有网卡
        socket.setCallback(this);   // 将当前应用实例注册为 socket 的回调处理对象
        socket.setMulticastLoop(false);
        
        // 解析组播转发接口列表，匹配静态组播路由配置
        const char *multicastIfs = par("multicastInterfaces");
        if (multicastIfs && strlen(multicastIfs) > 0) {
            IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
            if (ift != nullptr) {
                // 按空格分割接口名称
                std::string interfacesStr(multicastIfs);
                std::istringstream iss(interfacesStr);
                std::string ifName;
                while (iss >> ifName) {
                    NetworkInterface *ie = ift->findInterfaceByName(ifName.c_str());
                    if (ie != nullptr) {
                        multicastInterfaceIds.push_back(ie->getInterfaceId());
                        EV_INFO << "Added multicast output interface: " << ifName 
                                << " (id=" << ie->getInterfaceId() << ")" << endl;
                    } else {
                        EV_ERROR << "Warning: Interface '" << ifName << "' not found!" << endl;
                    }
                }
            }
        }
        
        // 解析按算力类型配置的组播组地址
        parseMulticastGroups(par("cpuMulticastGroups"), 0);  // 0 = CPU
        parseMulticastGroups(par("gpuMulticastGroups"), 1);  // 1 = GPU
        
        // 打印配置信息
        for (auto& entry : computingTypeMulticastGroups) {
            EV_INFO << "Computing type " << entry.first << " multicast groups: ";
            for (auto& addr : entry.second) {
                EV_INFO << addr << " ";
            }
            EV_INFO << endl;
        }
        
        // 设置 TTL，防止组播包在网络中无限转发
        socket.setTimeToLive(32);
    }
}

void UserGatewayApp::parseMulticastGroups(const char *groupsStr, int computingType)
{
    if (groupsStr && strlen(groupsStr) > 0) {
        std::string groups(groupsStr);
        std::istringstream iss(groups);
        std::string addrStr;
        while (iss >> addrStr) {
            L3Address addr = L3AddressResolver().resolve(addrStr.c_str());
            if (!addr.isUnspecified()) {
                computingTypeMulticastGroups[computingType].push_back(addr);
                EV_INFO << "Added multicast group " << addr 
                        << " for computing type " << computingType << endl;
            } else {
                EV_ERROR << "Warning: Invalid multicast address '" << addrStr << "'" << endl;
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
    
    // 获取对应算力类型的组播组地址列表
    auto it = computingTypeMulticastGroups.find(computingType);
    if (it == computingTypeMulticastGroups.end() || it->second.empty()) {
        EV_ERROR << "No multicast groups configured for computing type " << computingType << endl;
        delete packet;
        return;
    }
    
    const std::vector<inet::L3Address>& targetGroups = it->second;
    
    EV_INFO << "Forwarding CPRP request for task(" << payload->getUserId() << "-" << payload->getTaskId() 
            << ") computingType=" << computingType 
            << " to " << targetGroups.size() << " multicast group(s)" << endl;

    // 检查是否配置了组播转发接口
    if (!multicastInterfaceIds.empty()) {
        // 为每个组播组地址和每个接口创建数据包副本并发送
        for (const auto& groupAddr : targetGroups) {
            for (int interfaceId : multicastInterfaceIds) {
                Packet *pkt = new Packet(messageType.c_str());
                pkt->insertAtBack(payload);
                
                // 使用 InterfaceReq 标签强制指定输出接口
                auto interfaceReq = pkt->addTagIfAbsent<InterfaceReq>();
                interfaceReq->setInterfaceId(interfaceId);
                
                // 设置目的地址和端口
                auto addressReq = pkt->addTagIfAbsent<L3AddressReq>();
                addressReq->setDestAddress(groupAddr);
                pkt->addTagIfAbsent<L4PortReq>()->setDestPort(computeGatewayPort);
                
                // 发送
                socket.sendTo(pkt, groupAddr, computeGatewayPort);
                
                EV_INFO << "Sent to group " << groupAddr 
                        << " via interfaceId=" << interfaceId << endl;
            }
        }
    } else {
        // 没有配置特定接口，使用默认组播发送
        for (const auto& groupAddr : targetGroups) {
            Packet *pkt = new Packet(messageType.c_str());
            pkt->insertAtBack(payload);
            socket.sendTo(pkt, groupAddr, computeGatewayPort);
            
            EV_INFO << "Sent to group " << groupAddr << " (default interface)" << endl;
        }
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

    // 提取任务请求负载信息
    const auto& respInfo = packet->popAtFront<CprpResponseMsg>();

    if(respInfo == nullptr){
        EV_WARN << "Error: Received a Packet named '" << packet->getName()
                 << "', but it does not contain a CprpResponseMsg chunk. Discarding.";
        delete packet;
        return;
    }

    int uid = respInfo->getUserId();
    int tid = respInfo->getTaskId();

    auto& cpArray = cpMap[{uid,tid}];
    computeNodeInfo cpNodeInfo;
    cpNodeInfo.computeNodeAddress = respInfo->getComputeNodeAddress();
    cpArray.push_back(cpNodeInfo);

    EV_INFO << "Node info of CPRP_RESP has been recorded\n";

    delete packet;
}

// 发送算力确认消息
void UserGatewayApp::sendCprpConfirm(Packet *packet)
{
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(packet) << std::endl;

//    // 提取任务请求负载信息
//    const auto& comfInfo = packet->popAtFront<CprpConfirmMsg>();
//
//    if(comfInfo == nullptr){
//        EV_WARN << "Error: Received a Packet named '" << packet->getName()
//                 << "', but it does not contain a CprpConfirmMsg chunk. Discarding.";
//        delete packet;
//        return;
//    }
//
//    // 创建算力应答载荷
//    auto payload = makeShared<CprpConfirmMsg>();
//    payload->setUserId(comfInfo->getUserId());
//    payload->setTaskId(comfInfo->getTaskId());
//    payload->setSelectedNodeId(comfInfo->getSelectedNodeId());
//
//    std::string messageType = payload->getMsgType();
//    Packet *pkt = new Packet(messageType.c_str());
//    pkt->insertAtBack(payload);
//
//    socket.sendTo(pkt, comfInfo->, userGatewayPort);


    EV_INFO << "Node info of CPRP_RESP has been recorded\n";

    delete packet;
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
