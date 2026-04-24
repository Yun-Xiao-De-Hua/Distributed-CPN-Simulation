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

#ifndef INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_SESSIONMANAGER_H_
#define INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_SESSIONMANAGER_H_

#include <map>
#include <vector>
#include <omnetpp.h>
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/contract/IInterfaceTable.h"

namespace inet {

// 请求会话软状态
struct RequestSessionState {
    int userId;
    int taskId;
    L3Address computeGatewayAddress;
    int computeGatewayPort;
    
    simtime_t minTotalDelay;
    double totalDelay;
    L3Address computeNodeAddress;
    int computeNodeId;
    
    std::vector<L3Address> sidPath;
    
    simtime_t lastHopSendTime;
    simtime_t expiryTime;
    
    double requiredBandwidth;
    int interfaceId;
    L3Address userGatewayAddress;
    
    simtime_t computingDelay;
    simtime_t queuingDelay;
    simtime_t transmissionDelay;
    double computeCost;
    double computingCapacity;
    double availableStorage;
};

// 带宽预留记录
struct BandwidthReservation {
    int userId;
    int taskId;
    L3Address userGatewayAddress;
    double reservedBandwidth;
    simtime_t expiryTime;
    int interfaceId;
};

// 接口带宽状态
struct InterfaceBandwidthState {
    double totalBandwidth;
    double reservedBandwidth;
    std::map<std::pair<int, int>, BandwidthReservation> reservations;
};

class SessionManager : public cSimpleModule
{
protected:
    // 请求会话表: (userId, taskId) -> RequestSessionState
    std::map<std::pair<int, int>, RequestSessionState> sessionTable;
    
    // 接口带宽管理
    std::map<int, InterfaceBandwidthState> interfaceBandwidths;
    
    simtime_t sessionTimeout;
    simtime_t defaultReservationTimeout;
    bool strictPruning;

    cMessage *cleanupTimer = nullptr;

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;

    void cleanupExpiredSessions();
    void cleanupExpiredReservations();
    void scheduleCleanupTimer();

public:
    SessionManager();
    virtual ~SessionManager();

    // 会话管理
    bool hasSession(int userId, int taskId) const;
    const RequestSessionState* getSession(int userId, int taskId) const;
    RequestSessionState* getSessionForUpdate(int userId, int taskId);
    void createSession(const RequestSessionState& state);
    void updateSession(const RequestSessionState& state);
    void removeSession(int userId, int taskId);
    void refreshSession(int userId, int taskId);
    
    // 带宽管理
    void setInterfaceBandwidth(int interfaceId, double bandwidth);
    double getTotalBandwidth(int interfaceId) const;
    double getReservedBandwidth(int interfaceId) const;
    double getAvailableBandwidth(int interfaceId);
    
    bool tryReserveBandwidth(int interfaceId, int userId, int taskId,
                             const L3Address& userGwAddr,
                             double bandwidth, simtime_t duration);
    void releaseBandwidth(int interfaceId, int userId, int taskId);
    bool checkBandwidthAvailability(int interfaceId, double required);
};

} // namespace inet

#endif /* INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_SESSIONMANAGER_H_ */
