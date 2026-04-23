#ifndef INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_BANDWIDTHMANAGER_H_
#define INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_BANDWIDTHMANAGER_H_

#include <map>
#include <omnetpp.h>
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/contract/IInterfaceTable.h"

namespace inet {

struct BandwidthReservation {
    int userId;
    int taskId;
    L3Address userGatewayAddress;
    double reservedBandwidth;
    simtime_t expiryTime;
    int interfaceId;
};

struct InterfaceBandwidthState {
    double totalBandwidth;
    double reservedBandwidth;
    std::map<std::pair<int, int>, BandwidthReservation> reservations;
};

class BandwidthManager : public cSimpleModule
{
protected:
    std::map<int, InterfaceBandwidthState> interfaceBandwidths;
    simtime_t defaultReservationTimeout;
    bool strictPruning;

    cMessage *cleanupTimer = nullptr;

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;

    void cleanupExpiredReservations();
    void scheduleCleanupTimer();

public:
    BandwidthManager();
    virtual ~BandwidthManager();

    void setInterfaceBandwidth(int interfaceId, double bandwidth);
    double getTotalBandwidth(int interfaceId) const;
    double getReservedBandwidth(int interfaceId) const;
    double getAvailableBandwidth(int interfaceId);

    bool tryReserveBandwidth(int interfaceId, int userId, int taskId,
                             const L3Address& userGwAddr,
                             double bandwidth, simtime_t duration);
    void releaseBandwidth(int interfaceId, int userId, int taskId);
    bool checkBandwidthAvailability(int interfaceId, double required);

    bool hasReservation(int interfaceId, int userId, int taskId) const;
    const BandwidthReservation* getReservation(int interfaceId, int userId, int taskId) const;
};

} // namespace inet

#endif /* INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_BANDWIDTHMANAGER_H_ */
