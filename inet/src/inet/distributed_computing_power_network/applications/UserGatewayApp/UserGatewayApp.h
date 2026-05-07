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

struct RequestContext {
    L3Address userNodeAddress;
    simtime_t generationTime;
    int computingType;
    double requiredStorage;
    double computingAmount;
    double transferAmount;
    simtime_t totalDelayRequirement;
    double budget;
    double userMaxBandwidth;
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

    std::map<std::pair<int, int>, std::vector<PathInfo>> pathCache;
    std::map<std::pair<int, int>, RequestContext> requestContextCache;

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
    void stripCpnPathHeader(Packet *packet);
    void processCprpConfirm(Packet *packet);
    void forwardTaskData(int userId, int taskId, int selectedNodeId, const L3Address& selectedNodeAddress, int selectedNodePort, int computingType);
    void parseMulticastGroup(const char *groupStr, int computingType);
    void parseMulticastRoutes(const char *routesStr);

public:
    UserGatewayApp();
    virtual ~UserGatewayApp();
};

} /* namespace inet */

#endif /* INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_APPLICATIONS_USERGATEWAYAPP_USERGATEWAYAPP_H_ */
