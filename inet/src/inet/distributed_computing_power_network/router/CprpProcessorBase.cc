#include "CprpProcessorBase.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"
#include "inet/networklayer/common/DscpTag_m.h"
#include "inet/distributed_computing_power_network/message/cpn_message_m.h"

namespace inet {

Define_Module(CprpProcessorBase);

CprpProcessorBase::CprpProcessorBase() {}
CprpProcessorBase::~CprpProcessorBase() {}

void CprpProcessorBase::initialize(int stage) {
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        enabled = par("enabled");
        strictPruning = par("strictPruning");
    }
    else if (stage == INITSTAGE_NETWORK_LAYER) {
        if (enabled) {
            ipv4Module.reference(this, "ipv4Module", true);
            if (ipv4Module.isValid()) {
                ipv4Module->registerHook(0, this);
                EV_INFO << "CprpProcessorBase registered as Netfilter hook" << endl;
            }
        }
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        sessionManager.reference(this, "sessionManagerModule", false);
        interfaceTable.reference(this, "interfaceTableModule", true);
        
        EV_INFO << "CprpProcessorBase initialized, enabled=" << enabled 
                << " strictPruning=" << strictPruning << endl;
    }
}

void CprpProcessorBase::handleMessage(cMessage *msg) {
    delete msg;
}

INetfilter::IHook::Result CprpProcessorBase::datagramPreRoutingHook(Packet *packet) {
    if (!enabled) return ACCEPT;
    refreshSessionIfMatch(packet);
    return ACCEPT;
}

INetfilter::IHook::Result CprpProcessorBase::datagramForwardHook(Packet *packet) {
    return ACCEPT;
}

INetfilter::IHook::Result CprpProcessorBase::datagramPostRoutingHook(Packet *packet,
                                                                      const NetworkInterface *inIE,
                                                                      const NetworkInterface *& outIE,
                                                                      L3Address& nextHopAddr) {
    if (!enabled) return ACCEPT;

    const char *pktName = packet->getName();
    
    if (strcmp(pktName, "CPRP_RESP") == 0) {
        return processCprpResp(packet, inIE, outIE);
    }
    else if (strcmp(pktName, "CANCEL") == 0) {
        processCancelMsg(packet);
        return DROP;
    }
    
    auto pathReq = packet->findTag<CpnPathReq>();
    if (pathReq != nullptr) {
        int mode = pathReq->getMode();
        if (mode == PATH_RECORD_MODE) {
            return processPathRecordMode(packet, outIE);
        }
        else if (mode == PATH_USE_MODE) {
            return processPathUseMode(packet, outIE);
        }
    }

    return ACCEPT;
}

INetfilter::IHook::Result CprpProcessorBase::datagramLocalInHook(Packet *packet, 
                                                                   const NetworkInterface *inIE) {
    return ACCEPT;
}

INetfilter::IHook::Result CprpProcessorBase::datagramLocalOutHook(Packet *packet) {
    return ACCEPT;
}

void CprpProcessorBase::refreshSessionIfMatch(Packet *packet) {
    const char *pktName = packet->getName();
    int userId = -1, taskId = -1;
    
    if (strcmp(pktName, "CPRP_RESP") == 0) {
        auto resp = packet->peekAtFront<CprpResponseMsg>();
        if (resp) {
            userId = resp->getUserId();
            taskId = resp->getTaskId();
        }
    }
    
    if (userId >= 0 && taskId >= 0 && sessionManager.isValid()) {
        if (sessionManager->hasSession(userId, taskId)) {
            sessionManager->refreshSession(userId, taskId);
            EV_INFO << "Refreshed session for task (" << userId << "," << taskId << ")" << endl;
        }
    }
}

INetfilter::IHook::Result CprpProcessorBase::processCprpResp(Packet *packet,
                                                              const NetworkInterface *inIE,
                                                              const NetworkInterface *outIE) {
    if (!sessionManager.isValid() || !outIE) return ACCEPT;

    auto resp = packet->peekAtFront<CprpResponseMsg>();
    if (!resp) return ACCEPT;

    int userId = resp->getUserId();
    int taskId = resp->getTaskId();
    
    EV_INFO << "Processing CPRP_RESP for task (" << userId << "," << taskId << ")" << endl;

    RequestSessionState newState;
    extractSessionFromResp(newState, packet);
    newState.interfaceId = outIE->getInterfaceId();
    
    if (!sessionManager->hasSession(userId, taskId)) {
        sessionManager->createSession(newState);
        EV_INFO << "First RESP for task (" << userId << "," << taskId << "), created session" << endl;
        return ACCEPT;
    }
    
    RequestSessionState* existingState = sessionManager->getSessionForUpdate(userId, taskId);
    if (!existingState) return ACCEPT;

    bool shouldKeep = shouldKeepNewSession(*existingState, newState, outIE->getInterfaceId());
    
    if (shouldKeep) {
        EV_INFO << "New RESP is better, updating session" << endl;
        sendCancelMessages(*existingState, SENDER_COMPUTE_ROUTER);
        sessionManager->updateSession(newState);
        return ACCEPT;
    }
    else {
        EV_INFO << "Existing session is better, dropping new RESP" << endl;
        sendCancelMessages(newState, SENDER_COMPUTE_ROUTER);
        return DROP;
    }
}

bool CprpProcessorBase::shouldKeepNewSession(const RequestSessionState& existing,
                                              const RequestSessionState& newResp,
                                              int outInterfaceId) {
    simtime_t now = simTime();
    simtime_t networkDelay = now - newResp.lastHopSendTime;
    simtime_t roundTripDelay = networkDelay * 2;
    
    double newTotalDelay = newResp.totalDelay + roundTripDelay.dbl();
    
    EV_INFO << "Routing selection: networkDelay=" << networkDelay 
            << " roundTripDelay=" << roundTripDelay 
            << " newTotalDelay=" << newTotalDelay 
            << " existingTotalDelay=" << existing.totalDelay << endl;
    
    if (sessionManager.isValid()) {
        double availableBw = sessionManager->getAvailableBandwidth(outInterfaceId);
        if (availableBw < newResp.requiredBandwidth) {
            EV_INFO << "Bandwidth constraint not satisfied: required=" << newResp.requiredBandwidth
                    << " available=" << availableBw << endl;
            return false;
        }
    }
    
    if (existing.totalDelay < 0 || newTotalDelay < existing.totalDelay) {
        EV_INFO << "New delay " << newTotalDelay << " < existing " << existing.totalDelay << endl;
        return true;
    }
    
    return false;
}

void CprpProcessorBase::processCancelMsg(Packet *packet) {
    auto cancel = packet->peekAtFront<CancelMsg>();
    if (!cancel) return;
    
    int userId = cancel->getUserId();
    int taskId = cancel->getTaskId();
    L3Address computeNodeAddr = cancel->getComputeNodeAddress();
    int computeNodePort = cancel->getComputeNodePort();
    
    EV_INFO << "Processing CANCEL for task (" << userId << "," << taskId 
            << ") computeNode=" << computeNodeAddr << ":" << computeNodePort << endl;
    
    const RequestSessionState* session = sessionManager->getSession(userId, taskId);
    
    if (session) {
        if (session->computeNodeAddress == computeNodeAddr &&
            session->computeNodePort == computeNodePort) {
            sessionManager->removeSession(userId, taskId);
            EV_INFO << "Removed session for task (" << userId << "," << taskId << ")" << endl;
        } else {
            EV_INFO << "CANCEL does not match current session (current=" 
                    << session->computeNodeAddress << ":" << session->computeNodePort 
                    << "), ignoring" << endl;
        }
    } else {
        EV_INFO << "No session found for task (" << userId << "," << taskId << ")" << endl;
    }
}

void CprpProcessorBase::sendCancelMessages(const RequestSessionState& state, 
                                            CancelSenderType senderType) {
    EV_INFO << "Sending CANCEL messages for task (" << state.userId << "," << state.taskId 
            << ") computeNode=" << state.computeNodeAddress << ":" << state.computeNodePort
            << " to " << state.sidPath.size() << " path nodes" << endl;
    
    for (const auto& addr : state.sidPath) {
        auto cancel = makeShared<CancelMsg>();
        cancel->setUserId(state.userId);
        cancel->setTaskId(state.taskId);
        cancel->setComputeNodeAddress(state.computeNodeAddress);
        cancel->setComputeNodePort(state.computeNodePort);
        cancel->setSenderType(senderType);
        
        Packet *pkt = new Packet("CANCEL");
        pkt->insertAtBack(cancel);
        
        auto addrReq = pkt->addTagIfAbsent<L3AddressReq>();
        addrReq->setDestAddress(addr);
        
        ipv4Module->sendOut(pkt);
        
        EV_INFO << "Sent CANCEL to " << addr << endl;
    }
}

INetfilter::IHook::Result CprpProcessorBase::processPathRecordMode(Packet *packet, 
                                                                     const NetworkInterface *outIE) {
    auto pathReq = packet->findTagForUpdate<CpnPathReq>();
    if (pathReq == nullptr || outIE == nullptr) return ACCEPT;

    Ipv4Address outAddr = outIE->getProtocolData<Ipv4InterfaceData>()->getIPAddress();
    
    int hopCount = pathReq->getHopAddressArraySize();
    pathReq->setHopAddressArraySize(hopCount + 1);
    pathReq->setHopAddress(hopCount, outAddr);
    
    EV_INFO << "Recording hop " << hopCount << ": " << outAddr << endl;

    return ACCEPT;
}

INetfilter::IHook::Result CprpProcessorBase::processPathUseMode(Packet *packet, 
                                                                  const NetworkInterface *outIE) {
    auto pathReq = packet->findTagForUpdate<CpnPathReq>();
    if (pathReq == nullptr) return ACCEPT;

    int currentIndex = pathReq->getCurrentHopIndex();
    int sidCount = pathReq->getSidListArraySize();

    if (currentIndex >= sidCount) {
        EV_INFO << "Reached end of SID list" << endl;
        return ACCEPT;
    }

    L3Address nextSid = pathReq->getSidList(currentIndex);
    pathReq->setCurrentHopIndex(currentIndex + 1);

    auto l3AddrReq = packet->addTagIfAbsent<L3AddressReq>();
    l3AddrReq->setDestAddress(nextSid);

    EV_INFO << "Source routing: forwarding to " << nextSid 
            << " (hop " << (currentIndex + 1) << "/" << sidCount << ")" << endl;

    return ACCEPT;
}

void CprpProcessorBase::extractSessionFromResp(RequestSessionState& state, Packet *packet) {
    auto resp = packet->peekAtFront<CprpResponseMsg>();
    if (!resp) return;
    
    state.userId = resp->getUserId();
    state.taskId = resp->getTaskId();
    state.computeNodeAddress = resp->getComputeNodeAddress();
    state.computeNodePort = resp->getComputeNodePort();
    state.computeNodeId = resp->getComputeNodeId();
    
    state.totalDelay = resp->getAccumulatedDelay().dbl();
    state.minTotalDelay = -1;
    state.requiredBandwidth = resp->getRequiredBandwidth();
    
    state.lastHopSendTime = resp->getLastHopSendTime();
    state.computeCost = resp->getComputeCost();
    state.computingCapacity = resp->getComputingCapacity();
    state.availableStorage = resp->getAvailableStorage();
    state.userGatewayAddress = resp->getLastHopAddress();
    
    auto pathReq = packet->findTag<CpnPathReq>();
    if (pathReq) {
        state.sidPath.clear();
        for (int i = 0; i < pathReq->getHopAddressArraySize(); i++) {
            state.sidPath.push_back(pathReq->getHopAddress(i));
        }
    }
}

} // namespace inet
