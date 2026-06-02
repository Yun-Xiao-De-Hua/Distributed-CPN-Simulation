#include "SessionManager.h"
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/NetworkInterface.h"
#include "inet/networklayer/common/InterfaceTable.h"
#include <sstream>

namespace inet {

Define_Module(SessionManager);

SessionManager::SessionManager() {}
SessionManager::~SessionManager() {
    cancelAndDelete(sessionExpirationTimer);
}

void SessionManager::initialize(int stage) {
    cSimpleModule::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        sessionTimeout = par("sessionTimeout");
        defaultInterfaceBandwidth = par("defaultInterfaceBandwidth");
        sessionExpirationTimer = new cMessage("sessionExpirationTimer");
        EV_INFO << "SessionManager configured: sessionTimeout=" << sessionTimeout
                << ", defaultInterfaceBandwidth=" << defaultInterfaceBandwidth << " bps" << endl;
    }
    else if (stage == INITSTAGE_NETWORK_LAYER) {
        initializeInterfaceBandwidths();
    }
}

void SessionManager::initializeInterfaceBandwidths() {
    interfaceBandwidth.clear();
    reservedBandwidth.clear();

    interfaceTable = findModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
    if (!interfaceTable) {
        EV_WARN << "SessionManager cannot initialize interface bandwidths: interface table not found" << endl;
        return;
    }

    for (int i = 0; i < interfaceTable->getNumInterfaces(); i++) {
        auto ie = interfaceTable->getInterface(i);
        if (!ie || ie->isLoopback())
            continue;

        double bandwidth = ie->getDatarate();
        if (bandwidth <= 0)
            bandwidth = defaultInterfaceBandwidth;

        interfaceBandwidth[ie->getInterfaceId()] = bandwidth;
        reservedBandwidth[ie->getInterfaceId()] = 0;

        EV_INFO << "Initialized interface bandwidth: " << ie->getInterfaceName()
                << " id=" << ie->getInterfaceId()
                << " bandwidth=" << bandwidth << " bps" << endl;
    }
}

std::string SessionManager::getInterfaceLabel(int interfaceId) const {
    auto ie = interfaceTable == nullptr ? nullptr : interfaceTable->findInterfaceById(interfaceId);
    if (ie == nullptr)
        return std::string("id=") + std::to_string(interfaceId);

    int gateId = ie->getNodeOutputGateId();
    auto node = interfaceTable->getHostModule();
    cGate *gate = node != nullptr && gateId >= 0 ? node->gate(gateId) : nullptr;
    if (gate != nullptr) {
        std::ostringstream os;
        os << gate->getBaseName();
        if (gate->isVector())
            os << gate->getIndex();
        return os.str();
    }

    std::ostringstream os;
    os << ie->getInterfaceName() << "(id=" << interfaceId << ")";
    return os.str();
}

void SessionManager::handleMessage(cMessage *msg) {
    if (msg == sessionExpirationTimer) {
        removeExpiredSessions();
    }
    else {
        delete msg;
    }
}

void SessionManager::finish() {
    printSessionTable();
}

bool SessionManager::hasSession(int userId, int taskId) {
    return sessionTable.find({userId, taskId}) != sessionTable.end();
}

const RequestSessionState* SessionManager::getSession(int userId, int taskId) {
    auto it = sessionTable.find({userId, taskId});
    if (it != sessionTable.end()) {
        return &(it->second);
    }
    return nullptr;
}

RequestSessionState* SessionManager::getSessionForUpdate(int userId, int taskId) {
    auto it = sessionTable.find({userId, taskId});
    if (it != sessionTable.end()) {
        return &(it->second);
    }
    return nullptr;
}

void SessionManager::createSession(const RequestSessionState& state) {
    auto key = std::make_pair(state.userId, state.taskId);
    
    if (hasSession(state.userId, state.taskId)) {
        EV_WARN << "Session already exists for task (" << state.userId 
                << "," << state.taskId << "), use updateSession instead" << endl;
        return;
    }
    
    if (canReserveBandwidth(state.interfaceId, state.requiredBandwidth)) {
        reserveBandwidth(state.interfaceId, state.requiredBandwidth);
        
        sessionTable[key] = state;
        sessionTable[key].createTime = simTime();
        sessionTable[key].updateTime = simTime();
        
        EV_INFO << "Created session for task (" << state.userId << "," << state.taskId 
                << ") computeNode=" << state.computeNodeAddress 
                << ":" << state.computeNodePort << endl;
        scheduleNextExpirationCheck();
    } else {
        EV_WARN << "Cannot create session: insufficient bandwidth on interface "
                << getInterfaceLabel(state.interfaceId) << endl;
    }
}

void SessionManager::updateSession(const RequestSessionState& state) {
    auto key = std::make_pair(state.userId, state.taskId);
    
    if (!hasSession(state.userId, state.taskId)) {
        EV_WARN << "Session not found for task (" << state.userId 
                << "," << state.taskId << "), use createSession instead" << endl;
        return;
    }
    
    RequestSessionState& oldState = sessionTable[key];
    
    if (oldState.interfaceId != state.interfaceId || 
        oldState.requiredBandwidth != state.requiredBandwidth) {
        
        releaseBandwidth(oldState.interfaceId, oldState.requiredBandwidth);
        
        if (canReserveBandwidth(state.interfaceId, state.requiredBandwidth)) {
            reserveBandwidth(state.interfaceId, state.requiredBandwidth);
        } else {
            EV_WARN << "Cannot update session: insufficient bandwidth on interface "
                    << getInterfaceLabel(state.interfaceId) << endl;
            reserveBandwidth(oldState.interfaceId, oldState.requiredBandwidth);
            return;
        }
    }
    
    sessionTable[key] = state;
    sessionTable[key].updateTime = simTime();
    
    EV_INFO << "Updated session for task (" << state.userId << "," << state.taskId 
            << ") computeNode=" << state.computeNodeAddress 
            << ":" << state.computeNodePort << endl;
    scheduleNextExpirationCheck();
}

void SessionManager::removeSession(int userId, int taskId) {
    auto it = sessionTable.find({userId, taskId});
    if (it == sessionTable.end()) {
        EV_WARN << "Session not found for task (" << userId << "," << taskId << ")" << endl;
        return;
    }
    
    releaseBandwidth(it->second.interfaceId, it->second.requiredBandwidth);
    
    EV_INFO << "Removed session for task (" << userId << "," << taskId 
            << ") computeNode=" << it->second.computeNodeAddress 
            << ":" << it->second.computeNodePort << endl;
    
    sessionTable.erase(it);
    scheduleNextExpirationCheck();
}

void SessionManager::reserveBandwidth(int interfaceId, double bandwidth) {
    reservedBandwidth[interfaceId] += bandwidth;
    EV_INFO << "Reserved " << bandwidth << " bps on interface " << getInterfaceLabel(interfaceId)
            << ", total reserved: " << reservedBandwidth[interfaceId] << endl;
}

void SessionManager::releaseBandwidth(int interfaceId, double bandwidth) {
    if (reservedBandwidth[interfaceId] >= bandwidth) {
        reservedBandwidth[interfaceId] -= bandwidth;
        EV_INFO << "Released " << bandwidth << " bps on interface " << getInterfaceLabel(interfaceId)
                << ", total reserved: " << reservedBandwidth[interfaceId] << endl;
    } else {
        EV_WARN << "Cannot release more bandwidth than reserved on interface " 
                << getInterfaceLabel(interfaceId) << endl;
    }
}

double SessionManager::getAvailableBandwidth(int interfaceId) {
    auto totalIt = interfaceBandwidth.find(interfaceId);
    double total = totalIt != interfaceBandwidth.end() ? totalIt->second : defaultInterfaceBandwidth;
    double reserved = reservedBandwidth[interfaceId];
    return total - reserved;
}

bool SessionManager::canReserveBandwidth(int interfaceId, double bandwidth) {
    return getAvailableBandwidth(interfaceId) >= bandwidth;
}

void SessionManager::refreshSession(int userId, int taskId) {
    auto it = sessionTable.find({userId, taskId});
    if (it != sessionTable.end()) {
        it->second.updateTime = simTime();
        EV_INFO << "Refreshed session for task (" << userId << "," << taskId << ")" << endl;
        scheduleNextExpirationCheck();
    }
}

void SessionManager::scheduleNextExpirationCheck() {
    Enter_Method_Silent("scheduleNextExpirationCheck");

    if (sessionExpirationTimer == nullptr || sessionTimeout <= SIMTIME_ZERO)
        return;

    if (sessionTable.empty()) {
        if (sessionExpirationTimer->isScheduled())
            cancelEvent(sessionExpirationTimer);
        return;
    }

    simtime_t nextExpiration = SimTime::getMaxTime();
    for (const auto& pair : sessionTable) {
        simtime_t expirationTime = pair.second.updateTime + sessionTimeout;
        if (expirationTime < nextExpiration)
            nextExpiration = expirationTime;
    }

    if (nextExpiration < simTime())
        nextExpiration = simTime();

    if (sessionExpirationTimer->isScheduled())
        cancelEvent(sessionExpirationTimer);
    scheduleAt(nextExpiration, sessionExpirationTimer);
}

void SessionManager::removeExpiredSessions() {
    simtime_t now = simTime();
    std::vector<std::pair<int, int>> expiredKeys;
    
    for (auto& pair : sessionTable) {
        if (now - pair.second.updateTime >= sessionTimeout) {
            expiredKeys.push_back(pair.first);
        }
    }
    
    for (auto& key : expiredKeys) {
        EV_INFO << "Removing expired session for task (" << key.first 
                << "," << key.second << ")" << endl;
        removeSession(key.first, key.second);
    }
    scheduleNextExpirationCheck();
}

void SessionManager::printSessionTable() {
    EV_INFO << "=== Session Table ===" << endl;
    for (const auto& pair : sessionTable) {
        const RequestSessionState& s = pair.second;
        EV_INFO << "Task(" << s.userId << "," << s.taskId << ") "
                << "ComputeNode=" << s.computeNodeAddress << ":" << s.computeNodePort << " "
                << "Delay=" << s.totalDelay << " "
                << "Age=" << (simTime() - s.updateTime) << endl;
    }
}

} // namespace inet
