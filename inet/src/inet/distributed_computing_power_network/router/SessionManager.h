#ifndef INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_SESSIONMANAGER_H_
#define INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_SESSIONMANAGER_H_

#include <map>
#include <vector>
#include <omnetpp.h>
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/contract/IInterfaceTable.h"

namespace inet {

struct RequestSessionState {
    int userId;
    int taskId;
    
    L3Address computeNodeAddress;
    int computeNodePort;
    int computeNodeId;
    
    std::vector<L3Address> sidPath;
    int interfaceId;
    
    double totalDelay;
    double minTotalDelay;
    double requiredBandwidth;
    
    L3Address userGatewayAddress;
    simtime_t lastHopSendTime;
    double computeCost;
    double computingCapacity;
    double availableStorage;
    
    simtime_t createTime;
    simtime_t updateTime;
};

class SessionManager : public cSimpleModule
{
protected:
    std::map<std::pair<int, int>, RequestSessionState> sessionTable;
    
    std::map<int, double> interfaceBandwidth;
    std::map<int, double> reservedBandwidth;
    
    simtime_t sessionTimeout;
    
protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

public:
    SessionManager();
    virtual ~SessionManager();
    
    bool hasSession(int userId, int taskId);
    const RequestSessionState* getSession(int userId, int taskId);
    RequestSessionState* getSessionForUpdate(int userId, int taskId);
    
    void createSession(const RequestSessionState& state);
    void updateSession(const RequestSessionState& state);
    void removeSession(int userId, int taskId);
    
    void reserveBandwidth(int interfaceId, double bandwidth);
    void releaseBandwidth(int interfaceId, double bandwidth);
    double getAvailableBandwidth(int interfaceId);
    bool canReserveBandwidth(int interfaceId, double bandwidth);
    
    void refreshSession(int userId, int taskId);
    void removeExpiredSessions();
    void printSessionTable();

    size_t getSessionCount() const { return sessionTable.size(); }
};

} // namespace inet

#endif
