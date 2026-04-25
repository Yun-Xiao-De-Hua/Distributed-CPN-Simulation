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

struct PathInfo {
    std::vector<L3Address> sidPath;
    double totalDelay;
    double computeCost;
    double bandwidth;
    int computeNodeId;
    L3Address computeNodeAddress;
    int computeNodePort;
    simtime_t timestamp;
};

class UserGatewayApp: public ApplicationBase, public UdpSocket::ICallback{
protected:
    int userGatewayId;

    int localPort;
    int userNodePort;
    int computeGatewayPort;
    int computeNodePort;

    L3Address localAddress;

    std::map<int, inet::L3Address> userNodeIpMap;
    std::map<std::pair<int,int>,std::vector<computeNodeInfo>> cpMap;
    
    std::map<std::pair<int, int>, std::map<std::string, PathInfo>> pathCache;
    
    std::map<std::pair<int, int>, simtime_t> requestTimers;

    UdpSocket socket;
    
    std::map<inet::L3Address, std::vector<int>> multicastInterfacesMap;
    
    std::map<int, inet::L3Address> computingTypeMulticastGroup;

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;
    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;

    void sendCollectedNodeInfo(int userId, int taskId);
    void sendCprpRequest(Packet *packet);
    void startCprpRequestTimer(int userId, int taskId);
    void processCprpResp(Packet *packet);
    void processCprpConfirm(Packet *packet);
    void forwardTaskData(int userId, int taskId, int selectedNodeId, int computingType);
    void parseMulticastGroup(const char *groupStr, int computingType);
    void parseMulticastRoutes(const char *routesStr);
    
    bool isTimerExpired(int userId, int taskId);
    void sendCancelToPathNodes(int userId, int taskId, 
                                const L3Address& computeNodeAddr, int computeNodePort,
                                const std::vector<L3Address>& path);
    void processCancelMsg(Packet *packet);

public:
    UserGatewayApp();
    virtual ~UserGatewayApp();
};

} /* namespace inet */

#endif /* INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_APPLICATIONS_USERGATEWAYAPP_USERGATEWAYAPP_H_ */
