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

#include "SessionManager.h"
#include "inet/common/ModuleAccess.h"

namespace inet {

Define_Module(SessionManager);

SessionManager::SessionManager()
{
}

SessionManager::~SessionManager()
{
    cancelAndDelete(cleanupTimer);
}

void SessionManager::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        sessionTimeout = par("sessionTimeout");
        defaultReservationTimeout = par("defaultReservationTimeout");
        strictPruning = par("strictPruning");
        cleanupTimer = new cMessage("CleanupTimer");
        
        EV_INFO << "SessionManager initialized with sessionTimeout=" << sessionTimeout 
                << " strictPruning=" << strictPruning << endl;
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        scheduleCleanupTimer();
    }
}

void SessionManager::handleMessage(cMessage *msg)
{
    if (msg == cleanupTimer) {
        cleanupExpiredSessions();
        cleanupExpiredReservations();
        scheduleCleanupTimer();
    }
}

void SessionManager::refreshDisplay() const
{
    char buf[128];
    sprintf(buf, "Sessions: %zu, Interfaces: %zu", sessionTable.size(), interfaceBandwidths.size());
    getDisplayString().setTagArg("t", 0, buf);
}

void SessionManager::scheduleCleanupTimer()
{
    scheduleAt(simTime() + 10.0, cleanupTimer);
}

void SessionManager::cleanupExpiredSessions()
{
    simtime_t now = simTime();
    int cleanedCount = 0;
    
    std::vector<std::pair<int, int>> toRemove;
    for (auto& pair : sessionTable) {
        if (pair.second.expiryTime <= now) {
            toRemove.push_back(pair.first);
            cleanedCount++;
        }
    }
    
    for (auto& key : toRemove) {
        EV_INFO << "Expired session for (" << key.first << "," << key.second << ")" << endl;
        sessionTable.erase(key);
    }
    
    if (cleanedCount > 0) {
        EV_INFO << "Cleaned " << cleanedCount << " expired sessions" << endl;
    }
}

void SessionManager::cleanupExpiredReservations()
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

// 会话管理方法
bool SessionManager::hasSession(int userId, int taskId) const
{
    return sessionTable.find({userId, taskId}) != sessionTable.end();
}

const RequestSessionState* SessionManager::getSession(int userId, int taskId) const
{
    auto it = sessionTable.find({userId, taskId});
    if (it != sessionTable.end())
        return &it->second;
    return nullptr;
}

RequestSessionState* SessionManager::getSessionForUpdate(int userId, int taskId)
{
    auto it = sessionTable.find({userId, taskId});
    if (it != sessionTable.end())
        return &it->second;
    return nullptr;
}

void SessionManager::createSession(const RequestSessionState& state)
{
    sessionTable[{state.userId, state.taskId}] = state;
    sessionTable[{state.userId, state.taskId}].expiryTime = simTime() + sessionTimeout;
    
    EV_INFO << "Created session for (" << state.userId << "," << state.taskId << ")" << endl;
}

void SessionManager::updateSession(const RequestSessionState& state)
{
    auto it = sessionTable.find({state.userId, state.taskId});
    if (it != sessionTable.end()) {
        it->second = state;
        it->second.expiryTime = simTime() + sessionTimeout;
        
        EV_INFO << "Updated session for (" << state.userId << "," << state.taskId << ")" << endl;
    }
}

void SessionManager::removeSession(int userId, int taskId)
{
    auto it = sessionTable.find({userId, taskId});
    if (it != sessionTable.end()) {
        sessionTable.erase(it);
        EV_INFO << "Removed session for (" << userId << "," << taskId << ")" << endl;
    }
}

void SessionManager::refreshSession(int userId, int taskId)
{
    auto it = sessionTable.find({userId, taskId});
    if (it != sessionTable.end()) {
        it->second.expiryTime = simTime() + sessionTimeout;
        EV_INFO << "Refreshed session for (" << userId << "," << taskId << ")" << endl;
    }
}

// 带宽管理方法
void SessionManager::setInterfaceBandwidth(int interfaceId, double bandwidth)
{
    auto& state = interfaceBandwidths[interfaceId];
    state.totalBandwidth = bandwidth;
    
    EV_INFO << "Set interface " << interfaceId << " bandwidth to " << bandwidth << " bps" << endl;
}

double SessionManager::getTotalBandwidth(int interfaceId) const
{
    auto it = interfaceBandwidths.find(interfaceId);
    if (it != interfaceBandwidths.end())
        return it->second.totalBandwidth;
    return 0;
}

double SessionManager::getReservedBandwidth(int interfaceId) const
{
    auto it = interfaceBandwidths.find(interfaceId);
    if (it != interfaceBandwidths.end())
        return it->second.reservedBandwidth;
    return 0;
}

double SessionManager::getAvailableBandwidth(int interfaceId)
{
    auto it = interfaceBandwidths.find(interfaceId);
    if (it != interfaceBandwidths.end())
        return it->second.totalBandwidth - it->second.reservedBandwidth;
    return 0;
}

bool SessionManager::tryReserveBandwidth(int interfaceId, int userId, int taskId,
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
            << ") on interface " << interfaceId << endl;

    return true;
}

void SessionManager::releaseBandwidth(int interfaceId, int userId, int taskId)
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

bool SessionManager::checkBandwidthAvailability(int interfaceId, double required)
{
    double available = getAvailableBandwidth(interfaceId);
    return available >= required;
}

} // namespace inet
