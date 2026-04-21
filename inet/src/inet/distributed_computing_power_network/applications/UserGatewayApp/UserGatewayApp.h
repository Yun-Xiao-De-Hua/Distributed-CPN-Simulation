//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_APPLICATIONS_USERGATEWAYAPP_USERGATEWAYAPP_H_
#define INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_APPLICATIONS_USERGATEWAYAPP_USERGATEWAYAPP_H_

#include <map>
#include <vector>
#include <omnetpp.h>
#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/distributed_computing_power_network/message/cpn_message_m.h"

namespace inet {

class UserGatewayApp: public ApplicationBase, public UdpSocket::ICallback{
protected:
    int userGatewayId;

    int localPort;
    int userNodePort;
    int computeGatewayPort;
    int computeNodePort;

    L3Address localAddress;

    std::map<int, inet::L3Address> userNodeIpMap;  // 存储用户节点 ID -> IP 地址映射
    std::map<std::pair<int,int>,std::vector<computeNodeInfo>> cpMap;

    UdpSocket socket;
    
    // 组播地址 -> 转发接口ID列表的映射
    std::map<inet::L3Address, std::vector<int>> multicastInterfacesMap;
    
    // 算力类型 -> 组播组地址的映射 (每种算力类型只对应一个组播组地址)
    std::map<int, inet::L3Address> computingTypeMulticastGroup;

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;
    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    // UdpSocket::ICallback
    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;

    // 向用户节点发送可用算力节点信息
    void sendCollectedNodeInfo(int userId, int taskId);
    // 发送算力请求到指定算力类型的所有组播组
    void sendCprpRequest(Packet *packet);
    // 启动算力请求计时器
    void startCprpRequestTimer(int userId, int taskId);
    // 算力应答消息处理
    void processCprpResp(Packet *packet);
    // 发送算力确认消息
    void sendCprpConfirm(Packet *packet);
    // 解析组播组地址 (算力类型 -> 单个组播地址)
    void parseMulticastGroup(const char *groupStr, int computingType);
    // 解析组播路由配置 (组播地址 -> 接口列表)
    void parseMulticastRoutes(const char *routesStr);

public:
    UserGatewayApp();
    virtual ~UserGatewayApp();
};

} /* namespace inet */

#endif /* INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_APPLICATIONS_USERGATEWAYAPP_USERGATEWAYAPP_H_ */
