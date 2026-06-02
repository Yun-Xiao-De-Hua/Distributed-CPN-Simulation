#include "UserGatewayProcessor.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"
#include <cstring>

namespace inet {

Define_Module(UserGatewayProcessor);

INetfilter::IHook::Result UserGatewayProcessor::datagramPostRoutingHook(Packet *packet) {
    Enter_Method("datagramPostRoutingHook");

    Result result = CprpProcessorBase::datagramPostRoutingHook(packet);
    if (result != ACCEPT)
        return result;

    if (!enabled || !isCprpPacket(packet))
        return ACCEPT;

    auto cancel = getCancelMsg(packet);
    if (strcmp(packet->getName(), "CPRP_CANCEL") == 0 || cancel != nullptr) {
        EV_INFO << "UserGatewayProcessor: cleaning local session for outgoing CPRP_CANCEL." << endl;
        cleanupCancelSession(packet);
    }

    return ACCEPT;
}

INetfilter::IHook::Result UserGatewayProcessor::datagramLocalInHook(Packet *packet) {
    Enter_Method("datagramLocalInHook");
    if (!enabled) return ACCEPT;

    if (!isCprpPacket(packet)) return ACCEPT;

    const char *pktName = packet->getName();

    auto cancel = getCancelMsg(packet);
    if (strcmp(pktName, "CPRP_CANCEL") == 0 || cancel != nullptr) {
        cleanupCancelSession(packet);
        return ACCEPT;
    }

    auto respProbe = getCprpResp(packet);
    if (strcmp(pktName, "CPRP_RESP") == 0 || respProbe != nullptr) {
        processLocalCprpResp(packet);
    }

    // 用户网关本地入站阶段仍处于IPv4解封装之前，不在这里修改IPv4/UDP报文内容。
    // CpnPathHeader在UDP payload交给UserGatewayApp后剥离，避免破坏IPv4后续decapsulate流程。
    EV_INFO << "UserGatewayProcessor: Accepting packet; path header stripping is deferred to UserGatewayApp." << endl;
    return ACCEPT;
}

INetfilter::IHook::Result UserGatewayProcessor::processLocalCprpResp(Packet *packet) {
    auto interfaceInd = packet->findTag<InterfaceInd>();
    if (interfaceInd == nullptr) {
        EV_WARN << "UserGatewayProcessor: InterfaceInd tag is missing, cannot reserve ingress bandwidth for CPRP_RESP." << endl;
        return ACCEPT;
    }

    auto resp = getCprpResp(packet);
    if (resp == nullptr) {
        EV_WARN << "UserGatewayProcessor: CprpResponseMsg was not found in local-in packet." << endl;
        return ACCEPT;
    }

    int userId = resp->getUserId();
    int taskId = resp->getTaskId();
    int ingressInterfaceId = interfaceInd->getInterfaceId();

    RequestSessionState newState;
    extractSessionFromResp(newState, packet);
    newState.interfaceId = ingressInterfaceId;

    simtime_t now = simTime();
    simtime_t networkDelay = now - resp->getLastHopSendTime();
    if (networkDelay < SIMTIME_ZERO)
        networkDelay = SIMTIME_ZERO;
    simtime_t roundTripDelay = networkDelay * 2;
    simtime_t accumulatedDelay = resp->getAccumulatedDelay() + roundTripDelay;
    newState.totalDelay = accumulatedDelay.dbl();
    newState.lastHopSendTime = now;

    L3Address lastHopAddress = resp->getLastHopAddress();
    auto ingressInterface = interfaceTable != nullptr ? interfaceTable->findInterfaceById(ingressInterfaceId) : nullptr;
    auto ipv4Data = ingressInterface != nullptr ? ingressInterface->getProtocolData<Ipv4InterfaceData>() : nullptr;
    if (ipv4Data != nullptr)
        lastHopAddress = L3Address(ipv4Data->getIPAddress());
    newState.userGatewayAddress = lastHopAddress;

    B respOffset = getCprpRespOffset(packet);
    if (respOffset >= B(0)) {
        auto updatedResp = makeShared<CprpResponseMsg>(*resp);
        updatedResp->setAccumulatedDelay(accumulatedDelay);
        updatedResp->setLastHopSendTime(now);
        updatedResp->setLastHopAddress(lastHopAddress);
        packet->replaceDataAt(updatedResp, respOffset, resp->getChunkLength(), Chunk::PF_ALLOW_INCOMPLETE);
        adjustIpv4UdpHeaderLengths(packet, B(0));
    }

    if (!sessionManager) {
        EV_WARN << "UserGatewayProcessor: session manager is missing, skipping CPRP_RESP session update." << endl;
        return ACCEPT;
    }

    if (!sessionManager->hasSession(userId, taskId)) {
        sessionManager->createSession(newState);
        EV_INFO << "UserGatewayProcessor: created ingress session for CPRP_RESP task ("
                << userId << "," << taskId << ") on interface " << ingressInterfaceId << endl;
        return ACCEPT;
    }

    RequestSessionState *existingState = sessionManager->getSessionForUpdate(userId, taskId);
    if (existingState == nullptr)
        return ACCEPT;

    bool betterDelay = existingState->totalDelay < 0 || newState.totalDelay < existingState->totalDelay;
    if (betterDelay) {
        sessionManager->updateSession(newState);
        EV_INFO << "UserGatewayProcessor: updated ingress session for CPRP_RESP task ("
                << userId << "," << taskId << ") with better delay=" << newState.totalDelay << endl;
    }
    else if (existingState->computeNodeAddress == newState.computeNodeAddress &&
             existingState->computeNodePort == newState.computeNodePort) {
        sessionManager->refreshSession(userId, taskId);
    }
    else {
        EV_INFO << "UserGatewayProcessor: kept existing ingress session for CPRP_RESP task ("
                << userId << "," << taskId << "), existingDelay=" << existingState->totalDelay
                << ", newDelay=" << newState.totalDelay << endl;
    }

    return ACCEPT;
}

void UserGatewayProcessor::cleanupCancelSession(Packet *packet) {
    auto cancel = getCancelMsg(packet);
    if (cancel == nullptr)
        return;

    int userId = cancel->getUserId();
    int taskId = cancel->getTaskId();
    L3Address computeNodeAddr = cancel->getComputeNodeAddress();
    int computeNodePort = cancel->getComputeNodePort();

    if (!sessionManager) {
        EV_INFO << "UserGatewayProcessor: no session manager, cannot cleanup CPRP_CANCEL for task ("
                << userId << "," << taskId << ")" << endl;
        return;
    }

    const RequestSessionState* session = sessionManager->getSession(userId, taskId);
    if (session == nullptr) {
        EV_INFO << "UserGatewayProcessor: no session found for CPRP_CANCEL task ("
                << userId << "," << taskId << ")" << endl;
        return;
    }

    if (session->computeNodeAddress == computeNodeAddr && session->computeNodePort == computeNodePort) {
        sessionManager->removeSession(userId, taskId);
        EV_INFO << "UserGatewayProcessor: removed session for CPRP_CANCEL task ("
                << userId << "," << taskId << ")" << endl;
    }
    else {
        EV_INFO << "UserGatewayProcessor: CPRP_CANCEL does not match current session for task ("
                << userId << "," << taskId << "), current="
                << session->computeNodeAddress << ":" << session->computeNodePort
                << ", cancel=" << computeNodeAddr << ":" << computeNodePort << endl;
    }
}

} // namespace inet
