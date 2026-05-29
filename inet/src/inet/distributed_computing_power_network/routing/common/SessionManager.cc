#include "SessionManager.h"
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/NetworkInterface.h"

namespace inet {

Define_Module(SessionManager);

SessionManager::SessionManager() {}
SessionManager::~SessionManager() {}

void SessionManager::initialize(int stage) {
    cSimpleModule::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        sessionTimeout = par("sessionTimeout");
        defaultInterfaceBandwidth = par("defaultInterfaceBandwidth");
    }
    else if (stage == INITSTAGE_NETWORK_LAYER) {
        initializeInterfaceBandwidths();
    }
}

void SessionManager::initializeInterfaceBandwidths() {
    interfaceBandwidth.clear();
    reservedBandwidth.clear();

    IInterfaceTable *ift = findModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
    if (!ift) {
        EV_WARN << "SessionManager cannot initialize interface bandwidths: interface table not found" << endl;
        return;
    }

    for (int i = 0; i < ift->getNumInterfaces(); i++) {
        auto ie = ift->getInterface(i);
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

void SessionManager::handleMessage(cMessage *msg) {
    delete msg;
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
    if (state.interfaceId < 0) {
        EV_WARN << "Cannot create session: invalid reservation interface " << state.interfaceId
                << " for task (" << state.userId << "," << state.taskId << ")" << endl;
        return;
    }
    
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
                << ":" << state.computeNodePort
                << " reservationInterface=" << state.interfaceId << endl;
    } else {
        EV_WARN << "Cannot create session: insufficient bandwidth on interface " 
                << state.interfaceId << endl;
    }
}

void SessionManager::updateSession(const RequestSessionState& state) {
    auto key = std::make_pair(state.userId, state.taskId);
    if (state.interfaceId < 0) {
        EV_WARN << "Cannot update session: invalid reservation interface " << state.interfaceId
                << " for task (" << state.userId << "," << state.taskId << ")" << endl;
        return;
    }
    
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
            EV_WARN << "Cannot update session: insufficient bandwidth" << endl;
            reserveBandwidth(oldState.interfaceId, oldState.requiredBandwidth);
            return;
        }
    }
    
    sessionTable[key] = state;
    sessionTable[key].updateTime = simTime();
    
    EV_INFO << "Updated session for task (" << state.userId << "," << state.taskId 
            << ") computeNode=" << state.computeNodeAddress 
            << ":" << state.computeNodePort
            << " reservationInterface=" << state.interfaceId << endl;
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
}

void SessionManager::reserveBandwidth(int interfaceId, double bandwidth) {
    if (interfaceId < 0) {
        EV_WARN << "Cannot reserve bandwidth on invalid interface " << interfaceId << endl;
        return;
    }

    reservedBandwidth[interfaceId] += bandwidth;
    EV_INFO << "Reserved " << bandwidth << " bps on interface " << interfaceId 
            << ", total reserved: " << reservedBandwidth[interfaceId] << endl;
}

void SessionManager::releaseBandwidth(int interfaceId, double bandwidth) {
    if (interfaceId < 0) {
        EV_WARN << "Cannot release bandwidth on invalid interface " << interfaceId << endl;
        return;
    }

    if (reservedBandwidth[interfaceId] >= bandwidth) {
        reservedBandwidth[interfaceId] -= bandwidth;
        EV_INFO << "Released " << bandwidth << " bps on interface " << interfaceId 
                << ", total reserved: " << reservedBandwidth[interfaceId] << endl;
    } else {
        EV_WARN << "Cannot release more bandwidth than reserved on interface " 
                << interfaceId << endl;
    }
}

double SessionManager::getAvailableBandwidth(int interfaceId) {
    if (interfaceId < 0)
        return 0;

    auto totalIt = interfaceBandwidth.find(interfaceId);
    double total = totalIt != interfaceBandwidth.end() ? totalIt->second : defaultInterfaceBandwidth;
    double reserved = reservedBandwidth[interfaceId];
    return total - reserved;
}

bool SessionManager::canReserveBandwidth(int interfaceId, double bandwidth) {
    if (interfaceId < 0)
        return false;

    return getAvailableBandwidth(interfaceId) >= bandwidth;
}

void SessionManager::refreshSession(int userId, int taskId) {
    auto it = sessionTable.find({userId, taskId});
    if (it != sessionTable.end()) {
        it->second.updateTime = simTime();
        EV_INFO << "Refreshed session for task (" << userId << "," << taskId << ")" << endl;
    }
}

void SessionManager::removeExpiredSessions() {
    simtime_t now = simTime();
    std::vector<std::pair<int, int>> expiredKeys;
    
    for (auto& pair : sessionTable) {
        if (now - pair.second.updateTime > sessionTimeout) {
            expiredKeys.push_back(pair.first);
        }
    }
    
    for (auto& key : expiredKeys) {
        EV_INFO << "Removing expired session for task (" << key.first 
                << "," << key.second << ")" << endl;
        removeSession(key.first, key.second);
    }
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
