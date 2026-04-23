#include "BandwidthManager.h"
#include "inet/common/ModuleAccess.h"

namespace inet {

Define_Module(BandwidthManager);

BandwidthManager::BandwidthManager()
{
}

BandwidthManager::~BandwidthManager()
{
    cancelAndDelete(cleanupTimer);
}

void BandwidthManager::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        defaultReservationTimeout = par("defaultReservationTimeout");
        strictPruning = par("strictPruning");
        cleanupTimer = new cMessage("CleanupTimer");
        
        EV_INFO << "BandwidthManager initialized with defaultTimeout=" << defaultReservationTimeout 
                << " strictPruning=" << strictPruning << endl;
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        scheduleCleanupTimer();
    }
}

void BandwidthManager::handleMessage(cMessage *msg)
{
    if (msg == cleanupTimer) {
        cleanupExpiredReservations();
        scheduleCleanupTimer();
    }
}

void BandwidthManager::refreshDisplay() const
{
    char buf[128];
    sprintf(buf, "Interfaces: %zu", interfaceBandwidths.size());
    getDisplayString().setTagArg("t", 0, buf);
}

void BandwidthManager::scheduleCleanupTimer()
{
    scheduleAt(simTime() + 10.0, cleanupTimer);
}

void BandwidthManager::cleanupExpiredReservations()
{
    simtime_t now = simTime();
    int cleanedCount = 0;

    for (auto& ifPair : interfaceBandwidths) {
        auto& state = ifPair.second;
        std::vector<std::pair<int, int>> toRemove;

        for (auto& resPair : state.reservations) {
            if (resPair.second.expiryTime <= now) {
                toRemove.push_back(resPair.first);
                state.reservedBandwidth -= resPair.second.reservedBandwidth;
                cleanedCount++;
                
                EV_INFO << "Expired bandwidth reservation for flow (" 
                        << resPair.first.first << "," << resPair.first.second 
                        << ") on interface " << ifPair.first << endl;
            }
        }

        for (auto& key : toRemove) {
            state.reservations.erase(key);
        }
    }

    if (cleanedCount > 0) {
        EV_INFO << "Cleaned " << cleanedCount << " expired reservations" << endl;
    }
}

void BandwidthManager::setInterfaceBandwidth(int interfaceId, double bandwidth)
{
    auto& state = interfaceBandwidths[interfaceId];
    state.totalBandwidth = bandwidth;
    
    EV_INFO << "Set interface " << interfaceId << " bandwidth to " << bandwidth << " bps" << endl;
}

double BandwidthManager::getTotalBandwidth(int interfaceId) const
{
    auto it = interfaceBandwidths.find(interfaceId);
    if (it != interfaceBandwidths.end())
        return it->second.totalBandwidth;
    return 0;
}

double BandwidthManager::getReservedBandwidth(int interfaceId) const
{
    auto it = interfaceBandwidths.find(interfaceId);
    if (it != interfaceBandwidths.end())
        return it->second.reservedBandwidth;
    return 0;
}

double BandwidthManager::getAvailableBandwidth(int interfaceId)
{
    auto it = interfaceBandwidths.find(interfaceId);
    if (it != interfaceBandwidths.end())
        return it->second.totalBandwidth - it->second.reservedBandwidth;
    return 0;
}

bool BandwidthManager::tryReserveBandwidth(int interfaceId, int userId, int taskId,
                                           const L3Address& userGwAddr,
                                           double bandwidth, simtime_t duration)
{
    auto& state = interfaceBandwidths[interfaceId];
    
    double available = state.totalBandwidth - state.reservedBandwidth;
    
    if (available < bandwidth) {
        EV_WARN << "Insufficient bandwidth for flow (" << userId << "," << taskId 
                << ") on interface " << interfaceId 
                << ": required=" << bandwidth 
                << " available=" << available << endl;
        return false;
    }

    BandwidthReservation res;
    res.userId = userId;
    res.taskId = taskId;
    res.userGatewayAddress = userGwAddr;
    res.reservedBandwidth = bandwidth;
    res.expiryTime = simTime() + (duration > 0 ? duration : defaultReservationTimeout);
    res.interfaceId = interfaceId;

    state.reservations[{userId, taskId}] = res;
    state.reservedBandwidth += bandwidth;

    EV_INFO << "Reserved " << bandwidth << " bps for flow (" << userId << "," << taskId 
            << ") on interface " << interfaceId 
            << " until " << res.expiryTime << endl;

    return true;
}

void BandwidthManager::releaseBandwidth(int interfaceId, int userId, int taskId)
{
    auto it = interfaceBandwidths.find(interfaceId);
    if (it == interfaceBandwidths.end())
        return;

    auto& state = it->second;
    auto resIt = state.reservations.find({userId, taskId});
    if (resIt != state.reservations.end()) {
        state.reservedBandwidth -= resIt->second.reservedBandwidth;
        EV_INFO << "Released bandwidth for flow (" << userId << "," << taskId 
                << ") on interface " << interfaceId << endl;
        state.reservations.erase(resIt);
    }
}

bool BandwidthManager::checkBandwidthAvailability(int interfaceId, double required)
{
    double available = getAvailableBandwidth(interfaceId);
    return available >= required;
}

bool BandwidthManager::hasReservation(int interfaceId, int userId, int taskId) const
{
    auto it = interfaceBandwidths.find(interfaceId);
    if (it == interfaceBandwidths.end())
        return false;
    return it->second.reservations.find({userId, taskId}) != it->second.reservations.end();
}

const BandwidthReservation* BandwidthManager::getReservation(int interfaceId, int userId, int taskId) const
{
    auto it = interfaceBandwidths.find(interfaceId);
    if (it == interfaceBandwidths.end())
        return nullptr;
    
    auto resIt = it->second.reservations.find({userId, taskId});
    if (resIt != it->second.reservations.end())
        return &resIt->second;
    return nullptr;
}

} // namespace inet
