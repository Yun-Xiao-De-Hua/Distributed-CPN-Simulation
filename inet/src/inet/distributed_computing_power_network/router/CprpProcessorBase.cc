//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//

#include "CprpProcessorBase.h"
#include "CprpProtocol.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"
#include "inet/networklayer/common/DscpTag_m.h"
#include "inet/networklayer/common/HopLimitTag_m.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/transportlayer/udp/UdpHeader_m.h"
#include "inet/networklayer/ipv4/Ipv4Header_m.h"

namespace inet {

Define_Module(CprpProcessorBase);

CprpProcessorBase::CprpProcessorBase() {}
CprpProcessorBase::~CprpProcessorBase() {
    cancelAndDelete(sendCancelsMsg);
}

void CprpProcessorBase::initialize(int stage) {
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        enabled = par("enabled");
        strictPruning = par("strictPruning");
        sendCancelsMsg = new cMessage("sendCancels");
    }
    else if (stage == INITSTAGE_NETWORK_LAYER) {
        if (enabled) {
            ipv4Module.reference(this, "ipv4Module", true);
            if (ipv4Module) {
                ipv4Module->registerHook(0, this);
                EV_INFO << "CprpProcessorBase registered as Netfilter hook" << endl;
            }
            else{
                EV_INFO << "Fail to reference for ipv4Module\n";
            }
        }
        localAddress = getLocalAddress();

        registerProtocol(cprp::cprp, gate("ipOut"), gate("ipIn"));
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        sessionManager.reference(this, "sessionManagerModule", false);
        interfaceTable.reference(this, "interfaceTableModule", true);

        EV_INFO << "CprpProcessorBase initialized, enabled=" << enabled
                << " strictPruning=" << strictPruning
                << " localAddress=" << localAddress << endl;
    }
}

void CprpProcessorBase::handleMessage(cMessage *msg) {
    if (msg == sendCancelsMsg) {
        sendPendingCancels();
    }
    else if (msg->isPacket()) {
        auto packet = check_and_cast<Packet*>(msg);
        if (strcmp(packet->getName(), "CANCEL") == 0) {
            processCancelMsg(packet);
        }
        else {
            delete packet;
        }
    }
    else {
        delete msg;
    }
}

void CprpProcessorBase::refreshDisplay() const {
    char buf[80];
    sprintf(buf, "sessions: %zu", sessionManager ? sessionManager->getSessionCount() : 0);
    getDisplayString().setTagArg("t", 0, buf);
}

L3Address CprpProcessorBase::getLocalAddress() {
    if (!ipv4Module) return L3Address();

    IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
    if (!ift) return L3Address();

    for (int i = 0; i < ift->getNumInterfaces(); i++) {
        auto ie = ift->getInterface(i);
        if (!ie->isLoopback() && ie->getProtocolData<Ipv4InterfaceData>()) {
            return L3Address(ie->getProtocolData<Ipv4InterfaceData>()->getIPAddress());
        }
    }
    return L3Address();
}

INetfilter::IHook::Result CprpProcessorBase::datagramPreRoutingHook(Packet *packet) {
    Enter_Method("datagramPreRoutingHook");
    if (!enabled) return ACCEPT;

    refreshSessionIfMatch(packet);
    return ACCEPT;
}

INetfilter::IHook::Result CprpProcessorBase::datagramForwardHook(Packet *packet) {
    return ACCEPT;
}

INetfilter::IHook::Result CprpProcessorBase::datagramPostRoutingHook(Packet *packet) {
    Enter_Method("datagramPostRoutingHook");
    if (!enabled) return ACCEPT;

    const char *pktName = packet->getName();

    if (strcmp(pktName, "CPRP_RESP") == 0) {
        return processCprpResp(packet);
    }
    else if (strcmp(pktName, "CANCEL") == 0) {
        return processCancelMsg(packet);
    }

    auto pathReq = packet->findTag<CpnPathReq>();
    if (pathReq != nullptr) {
        int mode = pathReq->getMode();
        if (mode == PATH_RECORD_MODE) {
            return processPathRecordMode(packet);
        }
        else if (mode == PATH_USE_MODE) {
            return processPathUseMode(packet);
        }
    }

    return ACCEPT;
}

INetfilter::IHook::Result CprpProcessorBase::datagramLocalInHook(Packet *packet) {
    Enter_Method("datagramLocalInHook");
    if (!enabled) return ACCEPT;

    if (strcmp(packet->getName(), "CANCEL") == 0) {
        return processCancelMsg(packet);
    }

    return ACCEPT;
}

INetfilter::IHook::Result CprpProcessorBase::datagramLocalOutHook(Packet *packet) {
    return ACCEPT;
}

void CprpProcessorBase::refreshSessionIfMatch(Packet *packet) {
    const char *pktName = packet->getName();
    int userId = -1, taskId = -1;

    if (strcmp(pktName, "CPRP_RESP") == 0) {
        auto resp = getCprpResp(packet);
        if (resp) {
            userId = resp->getUserId();
            taskId = resp->getTaskId();
        }
    }

    if (userId >= 0 && taskId >= 0 && sessionManager) {
        if (sessionManager->hasSession(userId, taskId)) {
            sessionManager->refreshSession(userId, taskId);
            EV_INFO << "Refreshed session for task (" << userId << "," << taskId << ")" << endl;
        }
    }
}

INetfilter::IHook::Result CprpProcessorBase::processCprpResp(Packet *packet) {
    auto outIE = packet->findTag<InterfaceReq>();
    if (!sessionManager || !outIE) return ACCEPT;

    auto resp = getCprpResp(packet);
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
        EV_INFO << "New RESP is better, updating session, sending CANCEL to old path" << endl;

        auto upstreamNodes = getUpstreamNodes(existingState->sidPath);
        for (const auto& addr : upstreamNodes) {
            PendingCancel pc;
            pc.destAddr = addr;
            pc.userId = existingState->userId;
            pc.taskId = existingState->taskId;
            pc.computeNodeAddress = existingState->computeNodeAddress;
            pc.computeNodePort = existingState->computeNodePort;
            pc.senderType = SENDER_COMPUTE_ROUTER;
            pendingCancels.push_back(pc);
        }

        if (!pendingCancels.empty() && !sendCancelsMsg->isScheduled()) {
            scheduleAt(simTime(), sendCancelsMsg);
        }

        sessionManager->updateSession(newState);
        return ACCEPT;
    }
    else {
        EV_INFO << "Existing session is better, dropping new RESP, sending CANCEL to new path" << endl;

        auto upstreamNodes = getUpstreamNodes(newState.sidPath);
        for (const auto& addr : upstreamNodes) {
            PendingCancel pc;
            pc.destAddr = addr;
            pc.userId = newState.userId;
            pc.taskId = newState.taskId;
            pc.computeNodeAddress = newState.computeNodeAddress;
            pc.computeNodePort = newState.computeNodePort;
            pc.senderType = SENDER_COMPUTE_ROUTER;
            pendingCancels.push_back(pc);
        }

        if (!pendingCancels.empty() && !sendCancelsMsg->isScheduled()) {
            scheduleAt(simTime(), sendCancelsMsg);
        }

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

    if (sessionManager) {
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

INetfilter::IHook::Result CprpProcessorBase::processCancelMsg(Packet *packet) {
    auto cancel = getCancelMsg(packet);
    if (!cancel) return ACCEPT;

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

    auto ipv4Header = packet->peekAtFront<Ipv4Header>();
    if (ipv4Header && ipv4Header->getProtocol() == &Protocol::udp) {
        EV_INFO << "CANCEL is UDP, ACCEPTing to pass to application layer" << endl;
        return ACCEPT;
    }

    return DROP;
}

void CprpProcessorBase::sendPendingCancels() {
    EV_INFO << "Sending " << pendingCancels.size() << " pending CANCEL messages" << endl;

    for (const auto& info : pendingCancels) {
        sendCancelPacket(info);
    }
    pendingCancels.clear();
}

void CprpProcessorBase::sendCancelPacket(const PendingCancel& info) {
    auto cancel = makeShared<CancelMsg>();
    cancel->setUserId(info.userId);
    cancel->setTaskId(info.taskId);
    cancel->setComputeNodeAddress(info.computeNodeAddress);
    cancel->setComputeNodePort(info.computeNodePort);
    cancel->setSenderType(info.senderType);
    cancel->setChunkLength(B(20));

    Packet *packet = new Packet("CANCEL");
    packet->insertAtBack(cancel);

    packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&cprp::cprp);
    packet->addTagIfAbsent<L3AddressReq>()->setDestAddress(info.destAddr);
    packet->addTagIfAbsent<HopLimitReq>()->setHopLimit(64);
    packet->addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::ipv4);

    send(packet, "ipOut");

    EV_INFO << "Sent CANCEL to " << info.destAddr << " for task ("
            << info.userId << "," << info.taskId << ")" << endl;
}

std::vector<L3Address> CprpProcessorBase::getUpstreamNodes(const std::vector<L3Address>& sidPath) {
    std::vector<L3Address> upstream;

    if (sidPath.empty()) return upstream;

    int myIndex = -1;
    for (int i = 0; i < (int)sidPath.size(); i++) {
        if (sidPath[i] == localAddress) {
            myIndex = i;
            break;
        }
    }

    if (myIndex < 0) {
        EV_WARN << "Local address not found in SID path, returning all nodes" << endl;
        return sidPath;
    }

    for (int i = 0; i < myIndex; i++) {
        upstream.push_back(sidPath[i]);
    }

    EV_INFO << "Upstream nodes (" << upstream.size() << "): ";
    for (const auto& addr : upstream) {
        EV_INFO << addr << " ";
    }
    EV_INFO << endl;

    return upstream;
}

INetfilter::IHook::Result CprpProcessorBase::processPathRecordMode(Packet *packet) {
    auto outIE = packet->findTag<InterfaceReq>();
    auto pathReq = packet->findTagForUpdate<CpnPathReq>();
    if (pathReq == nullptr || !outIE) return ACCEPT;

    auto ie = interfaceTable->getInterfaceById(outIE->getInterfaceId());
    if (!ie) return ACCEPT;

    Ipv4Address outAddr = ie->getProtocolData<Ipv4InterfaceData>()->getIPAddress();

    int hopCount = pathReq->getHopAddressArraySize();
    pathReq->setHopAddressArraySize(hopCount + 1);
    pathReq->setHopAddress(hopCount, outAddr);

    EV_INFO << "Recording hop " << hopCount << ": " << outAddr << endl;

    return ACCEPT;
}

INetfilter::IHook::Result CprpProcessorBase::processPathUseMode(Packet *packet) {
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
    auto resp = getCprpResp(packet);
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

Ptr<const CprpResponseMsg> CprpProcessorBase::getCprpResp(Packet *packet) {
    auto ipv4Header = packet->peekAtFront<Ipv4Header>();
    if (!ipv4Header || ipv4Header->getProtocol() != &Protocol::udp) return nullptr;
    
    auto ipLen = B(ipv4Header->getHeaderLength());
    auto udpHeader = packet->peekDataAt<UdpHeader>(ipLen);
    if (!udpHeader) return nullptr;
    
    auto udpLen = B(udpHeader->getChunkLength());
    return packet->peekDataAt<CprpResponseMsg>(ipLen + udpLen);
}

Ptr<const CancelMsg> CprpProcessorBase::getCancelMsg(Packet *packet) {
    auto ipv4Header = packet->peekAtFront<Ipv4Header>();
    if (!ipv4Header) return nullptr;
    
    auto ipLen = B(ipv4Header->getHeaderLength());

    if (ipv4Header->getProtocol() == &cprp::cprp) {
        // Form 1: Raw IP (Network Layer CANCEL)
        return packet->peekDataAt<CancelMsg>(ipLen);
    } 
    else if (ipv4Header->getProtocol() == &Protocol::udp) {
        // Form 2: UDP (Application Layer CANCEL)
        auto udpHeader = packet->peekDataAt<UdpHeader>(ipLen);
        if (!udpHeader) return nullptr;
        
        auto udpLen = B(udpHeader->getChunkLength());
        return packet->peekDataAt<CancelMsg>(ipLen + udpLen);
    }

    return nullptr;
}

} // namespace inet
