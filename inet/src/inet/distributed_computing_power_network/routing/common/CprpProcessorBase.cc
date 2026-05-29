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
#include "inet/networklayer/common/NextHopAddressTag_m.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/transportlayer/udp/UdpHeader_m.h"
#include "inet/networklayer/ipv4/Ipv4Header_m.h"

namespace inet {

Define_Module(CprpProcessorBase);

static const B CPN_PATH_HEADER_BASE_LENGTH = B(64);
static const B CPN_PATH_ADDRESS_LENGTH = B(16);
static const int OPTIONAL_CHUNK_PEEK_FLAGS = Chunk::PF_ALLOW_NULLPTR | Chunk::PF_ALLOW_EMPTY | Chunk::PF_ALLOW_INCOMPLETE;
static const int OPTIONAL_TYPED_PEEK_FLAGS = Chunk::PF_ALLOW_NULLPTR | Chunk::PF_ALLOW_INCOMPLETE;
static const int OPTIONAL_REINTERPRET_TYPED_PEEK_FLAGS = OPTIONAL_TYPED_PEEK_FLAGS | Chunk::PF_ALLOW_REINTERPRETATION;

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
        cprp::registerCprpProtocol();
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
        if (strcmp(packet->getName(), "CPRP_CANCEL") == 0) {
            processCancelMsg(packet);
            delete packet;
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

    if (processIntermediatePathUseMode(packet))
        return ACCEPT;

    refreshSessionIfMatch(packet);
    return ACCEPT;
}

INetfilter::IHook::Result CprpProcessorBase::datagramForwardHook(Packet *packet) {
    return ACCEPT;
}

bool CprpProcessorBase::isCprpPacket(Packet *packet) {
    const char *name = packet->getName();
    if (strncmp(name, "CPRP_", 5) == 0 || strncmp(name, "TASK_", 5) == 0) {
        return true;
    }

    // CGMP 报文同样走 UDP 5000 端口，但不属于 CPRP 业务流，必须显式排除。
    if (strcmp(name, "CGMP_Query") == 0 || strcmp(name, "CGMP_Report") == 0) {
        return false;
    }
    
    auto ipv4Header = packet->peekAtFront<Ipv4Header>();
    if (ipv4Header) {
        if (ipv4Header->getProtocol() == &cprp::cprp) {
            return true;
        }
        if (ipv4Header->getProtocol() == &Protocol::udp) {
            auto ipLen = B(ipv4Header->getHeaderLength());
            if (b(ipLen + B(8)) >= packet->getDataLength())
                return false;
            auto udpHeader = packet->peekDataAt<UdpHeader>(ipLen, B(8), OPTIONAL_TYPED_PEEK_FLAGS);
            if (udpHeader && (udpHeader->getSourcePort() == 5000 || udpHeader->getDestinationPort() == 5000)) {
                return true;
            }
        }
    }
    return false;
}

Ptr<const CpnPathHeader> CprpProcessorBase::getCpnPathHeader(Packet *packet) {
    if (!isCprpPacket(packet)) return nullptr;

    B offset = getPayloadOffset(packet);
    if (offset < B(0)) return nullptr;
    if (b(offset + CPN_PATH_HEADER_BASE_LENGTH) > packet->getDataLength()) return nullptr;

    auto chunk = packet->peekDataAt<Chunk>(offset, b(-1), OPTIONAL_CHUNK_PEEK_FLAGS);
    auto pathHeader = dynamicPtrCast<const CpnPathHeader>(chunk);

    auto isValidPathHeader = [this, packet, offset](const Ptr<const CpnPathHeader>& header) {
        if (header == nullptr)
            return false;
        int mode = header->getMode();
        if (mode != PATH_RECORD_MODE && mode != PATH_USE_MODE)
            return false;
        B headerLength = B(header->getChunkLength());
        if (headerLength < getCpnPathHeaderLength(*header))
            return false;
        if (b(offset + headerLength) > packet->getDataLength())
            return false;
        return true;
    };

    if (isValidPathHeader(pathHeader))
        return pathHeader;

    try {
        pathHeader = packet->peekDataAt<CpnPathHeader>(offset, b(-1), OPTIONAL_REINTERPRET_TYPED_PEEK_FLAGS);
        if (isValidPathHeader(pathHeader))
            return pathHeader;
    }
    catch (const cRuntimeError& e) {
        EV_DEBUG << "getCpnPathHeader: no typed CpnPathHeader at UDP payload front: " << e.what() << endl;
    }

    return nullptr;
}

INetfilter::IHook::Result CprpProcessorBase::datagramPostRoutingHook(Packet *packet) {
    Enter_Method("datagramPostRoutingHook");
    if (!enabled) return ACCEPT;

    if (!isCprpPacket(packet)) return ACCEPT;

    const char *pktName = packet->getName();
    Result result = ACCEPT;

    // 1. 处理路径头部封装 (针对 UDP 数据包)
    handlePathHeader(packet);

    // 2. 统一处理路径模式 (Record / Use)
    auto pathReqTag = packet->findTag<CpnPathReq>();
    if (pathReqTag != nullptr) {
        // 优先使用本地 Tag (通常在源节点)
        int mode = pathReqTag->getMode();
        if (mode == PATH_RECORD_MODE) {
            processPathRecordMode(packet);
        }
        else if (mode == PATH_USE_MODE) {
            processPathUseMode(packet);
        }
    }
    else {
        // 物理Header的PATH_USE_MODE由datagramPreRoutingHook处理，避免在同一跳重复消费SID。
        auto pathHeader = getCpnPathHeader(packet);
        if (pathHeader != nullptr && pathHeader->getMode() == PATH_RECORD_MODE) {
            processPathRecordMode(packet);
        }
    }

    auto respProbe = getCprpResp(packet);
    if (strcmp(pktName, "CPRP_RESP") == 0 || respProbe != nullptr) {
        if (strcmp(pktName, "CPRP_RESP") != 0) {
            EV_INFO << "datagramPostRoutingHook: packet name is '" << pktName
                    << "', but CprpResponseMsg was detected in payload; treating it as CPRP_RESP." << endl;
        }
        result = processCprpResp(packet);
        if (result == DROP) return DROP;
    }

    return result;
}

INetfilter::IHook::Result CprpProcessorBase::datagramLocalInHook(Packet *packet) {
    Enter_Method("datagramLocalInHook");
    if (!enabled) return ACCEPT;

    if (!isCprpPacket(packet)) return ACCEPT;

    if (strcmp(packet->getName(), "CPRP_CANCEL") == 0) {
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

bool CprpProcessorBase::processIntermediatePathUseMode(Packet *packet) {
    if (!isCprpPacket(packet)) return false;

    auto ipv4Header = packet->peekAtFront<Ipv4Header>();
    if (!ipv4Header || ipv4Header->getProtocol() != &Protocol::udp)
        return false;

    if (!isLocalIpv4Address(ipv4Header->getDestAddress()))
        return false;

    auto pathHeader = getCpnPathHeader(packet);
    if (pathHeader == nullptr || pathHeader->getMode() != PATH_USE_MODE)
        return false;

    int currentIndex = pathHeader->getCurrentHopIndex();
    int sidCount = pathHeader->getSidListArraySize();
    if (currentIndex < 0 || currentIndex >= sidCount)
        return false;

    L3Address nextSid = pathHeader->getSidList(currentIndex);
    if (nextSid.isUnspecified())
        return false;

    B offset = getPayloadOffset(packet);
    if (offset < B(0))
        return false;

    auto newHeader = makeShared<CpnPathHeader>(*pathHeader);
    newHeader->setCurrentHopIndex(currentIndex + 1);
    replacePathHeader(packet, offset, newHeader, *pathHeader);

    if (!rewriteIpv4Destination(packet, nextSid))
        return false;

    clearForwardingDecisionTags(packet);

    EV_INFO << "Intermediate source routing: local SID reached, forwarding to " << nextSid
            << " (hop " << (currentIndex + 1) << "/" << sidCount << ")" << endl;
    return true;
}

bool CprpProcessorBase::isLocalIpv4Address(const Ipv4Address& address) const {
    if (address.isUnspecified() || interfaceTable == nullptr)
        return false;

    for (int i = 0; i < interfaceTable->getNumInterfaces(); i++) {
        auto ie = interfaceTable->getInterface(i);
        auto ipv4Data = ie->getProtocolData<Ipv4InterfaceData>();
        if (ipv4Data != nullptr && ipv4Data->getIPAddress() == address)
            return true;
    }

    return false;
}

bool CprpProcessorBase::rewriteIpv4Destination(Packet *packet, const L3Address& destination) {
    if (destination.getType() != L3Address::IPv4) {
        EV_WARN << "rewriteIpv4Destination: next SID is not an IPv4 address: " << destination << endl;
        return false;
    }

    auto ipv4Header = packet->peekAtFront<Ipv4Header>(B(20), OPTIONAL_REINTERPRET_TYPED_PEEK_FLAGS);
    if (!ipv4Header)
        return false;
    B ipHeaderLength = B(ipv4Header->getHeaderLength());
    if (ipHeaderLength < B(20) || b(ipHeaderLength) > packet->getDataLength()) {
        EV_WARN << "rewriteIpv4Destination: invalid IPv4 header length " << ipHeaderLength << endl;
        return false;
    }
    if (B(ipv4Header->getChunkLength()) < ipHeaderLength) {
        ipv4Header = packet->peekAtFront<Ipv4Header>(ipHeaderLength, OPTIONAL_REINTERPRET_TYPED_PEEK_FLAGS);
        if (!ipv4Header)
            return false;
    }
    B ipChunkLength = B(ipv4Header->getChunkLength());
    if (ipChunkLength < B(20) || b(ipChunkLength) > packet->getDataLength()) {
        EV_WARN << "rewriteIpv4Destination: invalid IPv4 chunk length " << ipChunkLength << endl;
        return false;
    }

    auto newIpv4Header = makeShared<Ipv4Header>(*ipv4Header);
    newIpv4Header->setDestAddress(destination.toIpv4());
    if (newIpv4Header->getCrcMode() == CRC_COMPUTED)
        newIpv4Header->updateCrc();

    packet->replaceDataAt(newIpv4Header, B(0), ipChunkLength, Chunk::PF_ALLOW_INCOMPLETE);

    auto networkProtocolInd = packet->addTagIfAbsent<NetworkProtocolInd>();
    networkProtocolInd->setProtocol(&Protocol::ipv4);
    networkProtocolInd->setNetworkProtocolHeader(newIpv4Header);
    packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&Protocol::ipv4);
    return true;
}

void CprpProcessorBase::clearForwardingDecisionTags(Packet *packet) {
    packet->removeTagIfPresent<L3AddressReq>();
    packet->removeTagIfPresent<InterfaceReq>();
    packet->removeTagIfPresent<NextHopAddressReq>();
}

INetfilter::IHook::Result CprpProcessorBase::processCprpResp(Packet *packet) {
    EV_INFO << "processCprpResp: packet='" << packet->getName()
            << "' totalLength=" << packet->getTotalLength()
            << " dataLength=" << packet->getDataLength() << endl;

    auto outIE = packet->findTag<InterfaceReq>();
    if (!sessionManager) {
        EV_WARN << "processCprpResp: sessionManager is missing, skipping RESP handling." << endl;
        return ACCEPT;
    }
    if (!outIE) {
        EV_WARN << "processCprpResp: InterfaceReq tag is missing, skipping RESP handling." << endl;
        return ACCEPT;
    }

    auto resp = getCprpResp(packet);
    if (!resp) {
        EV_WARN << "processCprpResp: CprpResponseMsg was not found at the expected offset." << endl;
        return ACCEPT;
    }

    int userId = resp->getUserId();
    int taskId = resp->getTaskId();

    EV_INFO << "Processing CPRP_RESP for task (" << userId << "," << taskId << ")" << endl;

    RequestSessionState newState;
    extractSessionFromResp(newState, packet);
    newState.interfaceId = outIE->getInterfaceId();

    simtime_t now = simTime();
    simtime_t networkDelay = now - resp->getLastHopSendTime();
    if (networkDelay < SIMTIME_ZERO)
        networkDelay = SIMTIME_ZERO;
    simtime_t roundTripDelay = networkDelay * 2;
    simtime_t accumulatedDelay = resp->getAccumulatedDelay() + roundTripDelay;
    newState.totalDelay = accumulatedDelay.dbl();
    newState.lastHopSendTime = now;

    L3Address lastHopAddress = resp->getLastHopAddress();
    auto currentInterface = interfaceTable ? interfaceTable->getInterfaceById(outIE->getInterfaceId()) : nullptr;
    if (currentInterface != nullptr) {
        auto ipv4Data = currentInterface->getProtocolData<Ipv4InterfaceData>();
        if (ipv4Data != nullptr)
            lastHopAddress = L3Address(ipv4Data->getIPAddress());
    }
    newState.userGatewayAddress = lastHopAddress;

    B respOffset = getCprpRespOffset(packet);
    if (respOffset >= B(0)) {
        auto updatedResp = makeShared<CprpResponseMsg>(*resp);
        updatedResp->setAccumulatedDelay(accumulatedDelay);
        updatedResp->setLastHopSendTime(now);
        updatedResp->setLastHopAddress(lastHopAddress);
        packet->replaceDataAt(updatedResp, respOffset, resp->getChunkLength(), OPTIONAL_TYPED_PEEK_FLAGS);
        adjustIpv4UdpHeaderLengths(packet, B(0));
    }

    EV_INFO << "processCprpResp: networkDelay=" << networkDelay
            << " roundTripDelay=" << roundTripDelay
            << " accumulatedDelay=" << accumulatedDelay << endl;

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
            if (addr == existingState->computeNodeAddress) {
                EV_INFO << "Skipping CPRP_CANCEL to compute node " << addr
                        << " for task (" << existingState->userId << "," << existingState->taskId << ")" << endl;
                continue;
            }

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
            if (addr == newState.computeNodeAddress) {
                EV_INFO << "Skipping CPRP_CANCEL to compute node " << addr
                        << " for task (" << newState.userId << "," << newState.taskId << ")" << endl;
                continue;
            }

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
    double newTotalDelay = newResp.totalDelay;

    EV_INFO << "Routing selection: newTotalDelay=" << newTotalDelay
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

    if (cancel->getSenderType() == SENDER_COMPUTE_GW) {
        EV_INFO << "CPRP_CANCEL sent by ComputeGateway, ACCEPTing to pass through" << endl;
        return ACCEPT;
    }

    int userId = cancel->getUserId();
    int taskId = cancel->getTaskId();
    L3Address computeNodeAddr = cancel->getComputeNodeAddress();
    int computeNodePort = cancel->getComputeNodePort();

    EV_INFO << "Processing CPRP_CANCEL for task (" << userId << "," << taskId
            << ") computeNode=" << computeNodeAddr << ":" << computeNodePort << endl;

    auto ipv4Header = packet->peekAtFront<Ipv4Header>();
    bool isUdpCancel = ipv4Header && ipv4Header->getProtocol() == &Protocol::udp;

    if (!sessionManager) {
        EV_INFO << "No session manager is configured, ignoring CPRP_CANCEL session cleanup" << endl;
        return DROP;
    }

    const RequestSessionState* session = sessionManager->getSession(userId, taskId);

    if (session) {
        if (session->computeNodeAddress == computeNodeAddr &&
            session->computeNodePort == computeNodePort) {
            sessionManager->removeSession(userId, taskId);
            EV_INFO << "Removed session for task (" << userId << "," << taskId << ")" << endl;
        } else {
            EV_INFO << "CPRP_CANCEL does not match current session (current="
                    << session->computeNodeAddress << ":" << session->computeNodePort
                    << "), ignoring" << endl;
        }
    } else {
        EV_INFO << "No session found for task (" << userId << "," << taskId << ")" << endl;
    }

    if (isUdpCancel) {
        EV_INFO << "CPRP_CANCEL is UDP, dropping after local session cleanup" << endl;
        return DROP;
    }

    return DROP;
}

void CprpProcessorBase::sendPendingCancels() {
    EV_INFO << "Sending " << pendingCancels.size() << " pending CPRP_CANCEL messages" << endl;

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

    Packet *packet = new Packet("CPRP_CANCEL");
    packet->insertAtBack(cancel);

    packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&cprp::cprp);
    packet->addTagIfAbsent<L3AddressReq>()->setDestAddress(info.destAddr);
    packet->addTagIfAbsent<HopLimitReq>()->setHopLimit(64);
    packet->addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::ipv4);

    send(packet, "ipOut");

    EV_INFO << "Sent CPRP_CANCEL to " << info.destAddr << " for task ("
            << info.userId << "," << info.taskId << ")" << endl;
}

INetfilter::IHook::Result CprpProcessorBase::processPathRecordMode(Packet *packet) {
    auto outIE = packet->findTag<InterfaceReq>();
    auto pathReqTag = packet->findTagForUpdate<CpnPathReq>();
    
    B offset = getPayloadOffset(packet);
    Ptr<const CpnPathHeader> pathHeader = getCpnPathHeader(packet);

    if (pathReqTag == nullptr && pathHeader == nullptr) {
        EV_WARN << "processPathRecordMode: Neither CpnPathReq tag nor CpnPathHeader chunk is present, skipping recording." << endl;
        return ACCEPT;
    }
    
    if (!outIE) {
        EV_WARN << "processPathRecordMode: InterfaceReq tag is missing from packet, returning ACCEPT without recording." << endl;
        return ACCEPT;
    }

    auto ie = interfaceTable->getInterfaceById(outIE->getInterfaceId());
    if (!ie) {
        EV_WARN << "processPathRecordMode: Output interface with ID " << outIE->getInterfaceId() 
                << " not found in interfaceTable." << endl;
        return ACCEPT;
    }

    auto ipv4Data = ie->getProtocolData<Ipv4InterfaceData>();
    if (!ipv4Data) {
        EV_WARN << "processPathRecordMode: Interface '" << ie->getInterfaceName() 
                << "' has no IPv4 configuration." << endl;
        return ACCEPT;
    }

    Ipv4Address outAddr = ipv4Data->getIPAddress();

    // 更新 Tag (如果是本地包)
    if (pathReqTag != nullptr) {
        int hopCount = pathReqTag->getHopAddressArraySize();
        pathReqTag->setHopAddressArraySize(hopCount + 1);
        pathReqTag->setHopAddress(hopCount, outAddr);
        EV_INFO << "processPathRecordMode: Recorded hop " << hopCount << " as " << outAddr << " in local Tag." << endl;
    }
    
    // 更新 Header (如果是转发包或本地已封装包)
    if (pathHeader != nullptr) {
        auto newHeader = makeShared<CpnPathHeader>(*pathHeader);
        int hopCount = newHeader->getHopAddressArraySize();
        newHeader->setHopAddressArraySize(hopCount + 1);
        newHeader->setHopAddress(hopCount, outAddr);
        replacePathHeader(packet, offset, newHeader, *pathHeader);
        EV_INFO << "processPathRecordMode: Recorded hop " << hopCount << " as " << outAddr << " in Header chunk." << endl;
    }

    return ACCEPT;
}

INetfilter::IHook::Result CprpProcessorBase::processPathUseMode(Packet *packet) {
    auto pathReqTag = packet->findTagForUpdate<CpnPathReq>();
    B offset = getPayloadOffset(packet);
    Ptr<const CpnPathHeader> pathHeader = getCpnPathHeader(packet);

    if (pathReqTag == nullptr && pathHeader == nullptr) return ACCEPT;

    int currentIndex = 0;
    int sidCount = 0;
    L3Address nextSid;

    if (pathReqTag != nullptr) {
        currentIndex = pathReqTag->getCurrentHopIndex();
        sidCount = pathReqTag->getSidListArraySize();
        if (currentIndex < sidCount) {
            nextSid = pathReqTag->getSidList(currentIndex);
            pathReqTag->setCurrentHopIndex(currentIndex + 1);
            if (pathHeader != nullptr) {
                // 本地Tag不会随报文跨节点传输，必须同步更新物理Header中的SID游标。
                auto newHeader = makeShared<CpnPathHeader>(*pathHeader);
                newHeader->setCurrentHopIndex(currentIndex + 1);
                replacePathHeader(packet, offset, newHeader, *pathHeader);
            }
        }
    }
    else if (pathHeader != nullptr) {
        currentIndex = pathHeader->getCurrentHopIndex();
        sidCount = pathHeader->getSidListArraySize();
        if (currentIndex < sidCount) {
            nextSid = pathHeader->getSidList(currentIndex);
            auto newHeader = makeShared<CpnPathHeader>(*pathHeader);
            newHeader->setCurrentHopIndex(currentIndex + 1);
            replacePathHeader(packet, offset, newHeader, *pathHeader);
        }
    }

    if (!nextSid.isUnspecified()) {
        auto l3AddrReq = packet->addTagIfAbsent<L3AddressReq>();
        l3AddrReq->setDestAddress(nextSid);
        EV_INFO << "Source routing: forwarding to " << nextSid
                << " (hop " << (currentIndex + 1) << "/" << sidCount << ")" << endl;
    }

    return ACCEPT;
}

void CprpProcessorBase::handlePathHeader(Packet *packet) {
    auto ipv4Header = packet->peekAtFront<Ipv4Header>();
    if (!ipv4Header || ipv4Header->getProtocol() != &Protocol::udp) return;

    if (!isCprpPacket(packet)) return;

    // 如果包中有 CpnPathReq 标签，但 UDP 负载中还没有 CpnPathHeader，则进行封装
    auto pathTag = packet->findTag<CpnPathReq>();
    if (pathTag == nullptr) return;

    B offset = getPayloadOffset(packet);
    if (offset < B(0)) return;

    auto existingHeader = getCpnPathHeader(packet);
    if (existingHeader == nullptr) {
        EV_INFO << "handlePathHeader: Encapsulating CpnPathReq Tag into Header chunk." << endl;
        auto pathHeader = makeShared<CpnPathHeader>();
        pathHeader->setMode(pathTag->getMode());
        pathHeader->setUserId(pathTag->getUserId());
        pathHeader->setTaskId(pathTag->getTaskId());
        pathHeader->setHopAddressArraySize(pathTag->getHopAddressArraySize());
        for (int i = 0; i < (int)pathTag->getHopAddressArraySize(); i++) {
            pathHeader->setHopAddress(i, pathTag->getHopAddress(i));
        }
        pathHeader->setUserGatewayAddress(pathTag->getUserGatewayAddress());
        pathHeader->setRequiredBandwidth(pathTag->getRequiredBandwidth());
        pathHeader->setSidListArraySize(pathTag->getSidListArraySize());
        for (int i = 0; i < (int)pathTag->getSidListArraySize(); i++) {
            pathHeader->setSidList(i, pathTag->getSidList(i));
        }
        pathHeader->setCurrentHopIndex(pathTag->getCurrentHopIndex());
        updateCpnPathHeaderLength(pathHeader);
        
        // 插入到 UDP 负载的最前端
        packet->insertDataAt(pathHeader, offset);
        adjustIpv4UdpHeaderLengths(packet, pathHeader->getChunkLength());
    }
}

void CprpProcessorBase::stripPathHeader(Packet *packet) {
    if (!isCprpPacket(packet)) return;

    B offset = getPayloadOffset(packet);
    if (offset < B(0)) return;

    auto pathHeader = getCpnPathHeader(packet);
    if (pathHeader != nullptr) {
        EV_INFO << "stripPathHeader: Stripping CpnPathHeader and converting to CpnPathInd Tag." << endl;
        
        // 构造指示标签供应用层使用
        auto pathInd = packet->addTagIfAbsent<CpnPathInd>();
        pathInd->setUserId(pathHeader->getUserId());
        pathInd->setTaskId(pathHeader->getTaskId());
        pathInd->setHopAddressArraySize(pathHeader->getHopAddressArraySize());
        for (int i = 0; i < (int)pathHeader->getHopAddressArraySize(); i++) {
            pathInd->setHopAddress(i, pathHeader->getHopAddress(i));
        }
        pathInd->setHopCount(pathHeader->getHopAddressArraySize());
        pathInd->setReservedBandwidth(pathHeader->getRequiredBandwidth());

        // 从物理报文中剥离 Header
        packet->eraseDataAt(offset, pathHeader->getChunkLength());
        adjustIpv4UdpHeaderLengths(packet, B(0) - B(pathHeader->getChunkLength()));
    }
}

B CprpProcessorBase::getPayloadOffset(Packet *packet) {
    auto ipv4Header = packet->peekAtFront<Ipv4Header>();
    if (!ipv4Header) return B(-1);
    
    auto ipLen = B(ipv4Header->getHeaderLength());
    if (ipv4Header->getProtocol() == &Protocol::udp) {
        return ipLen + B(8); // IPv4头 + UDP头(8字节)
    }
    return B(-1);
}

B CprpProcessorBase::getCpnPathHeaderLength(const CpnPathHeader& pathHeader) const {
    int addressCount = pathHeader.getHopAddressArraySize() + pathHeader.getSidListArraySize();
    return B(CPN_PATH_HEADER_BASE_LENGTH.get() + CPN_PATH_ADDRESS_LENGTH.get() * addressCount);
}

void CprpProcessorBase::updateCpnPathHeaderLength(const Ptr<CpnPathHeader>& pathHeader) const {
    pathHeader->setChunkLength(getCpnPathHeaderLength(*pathHeader));
}

void CprpProcessorBase::replacePathHeader(Packet *packet, B offset, const Ptr<CpnPathHeader>& newHeader, const CpnPathHeader& oldHeader) {
    updateCpnPathHeaderLength(newHeader);
    B delta = B(newHeader->getChunkLength()) - B(oldHeader.getChunkLength());
    packet->eraseDataAt(offset, oldHeader.getChunkLength());
    packet->insertDataAt(newHeader, offset);
    adjustIpv4UdpHeaderLengths(packet, delta);
}

void CprpProcessorBase::adjustIpv4UdpHeaderLengths(Packet *packet, B delta) {
    auto ipv4Header = packet->peekAtFront<Ipv4Header>();
    if (!ipv4Header || ipv4Header->getProtocol() != &Protocol::udp) return;

    auto ipHeaderLength = B(ipv4Header->getHeaderLength());
    if (b(ipHeaderLength + B(8)) > packet->getDataLength()) {
        EV_WARN << "adjustIpv4UdpHeaderLengths: packet too short for UDP header, delta=" << delta << endl;
        return;
    }

    auto udpHeader = packet->peekDataAt<UdpHeader>(ipHeaderLength, B(8), OPTIONAL_REINTERPRET_TYPED_PEEK_FLAGS);
    if (!udpHeader) {
        EV_WARN << "adjustIpv4UdpHeaderLengths: UDP header not found, delta=" << delta << endl;
        return;
    }

    auto newIpv4Header = makeShared<Ipv4Header>(*ipv4Header);
    auto newUdpHeader = makeShared<UdpHeader>(*udpHeader);

    B oldIpTotalLength = ipv4Header->getTotalLengthField();
    B oldUdpTotalLength = udpHeader->getTotalLengthField();
    B udpHeaderLength = B(udpHeader->getChunkLength());
    B newIpTotalLength = B(packet->getDataLength());
    B newUdpTotalLength = newIpTotalLength - ipHeaderLength;
    if (newIpTotalLength < ipHeaderLength + udpHeaderLength || newUdpTotalLength < udpHeaderLength) {
        EV_WARN << "adjustIpv4UdpHeaderLengths: refusing invalid length update, delta=" << delta
                << " ipTotal=" << newIpTotalLength << " udpTotal=" << newUdpTotalLength << endl;
        return;
    }

    bool ipLengthChanged = oldIpTotalLength != newIpTotalLength;
    bool udpLengthChanged = oldUdpTotalLength != newUdpTotalLength;
    bool udpChecksumReset = false;

    if (ipLengthChanged) {
        newIpv4Header->setTotalLengthField(newIpTotalLength);
        if (newIpv4Header->getCrcMode() == CRC_COMPUTED)
            newIpv4Header->updateCrc();
    }

    if (udpLengthChanged)
        newUdpHeader->setTotalLengthField(newUdpTotalLength);

    if (newUdpHeader->getCrcMode() == CRC_COMPUTED) {
        // CPRP在网络层修改UDP负载时，UDP校验和可能已经按旧负载计算过。
        // 保持CRC_COMPUTED并将校验和置零：若后续UDP校验钩子仍会执行，则由钩子重新计算；
        // 若钩子已经执行过，INET的IPv4 UDP接收逻辑会将0视为禁用校验，从而避免误丢包。
        newUdpHeader->setCrc(0x0000);
        udpChecksumReset = true;
    }
    else if (newUdpHeader->getCrcMode() != CRC_DISABLED) {
        // 接收侧重解释出的UDP头可能是DECLARED_*状态；payload已被修改后该声明不再可信，且不能直接序列化。
        newUdpHeader->setCrcMode(CRC_DISABLED);
        newUdpHeader->setCrc(0x0000);
        udpChecksumReset = true;
    }

    if (!ipLengthChanged && !udpLengthChanged && !udpChecksumReset)
        return;

    if (ipLengthChanged)
        packet->replaceDataAt(newIpv4Header, B(0), ipv4Header->getChunkLength(), Chunk::PF_ALLOW_INCOMPLETE);
    if (udpLengthChanged || udpChecksumReset)
        packet->replaceDataAt(newUdpHeader, ipHeaderLength, udpHeader->getChunkLength(), Chunk::PF_ALLOW_INCOMPLETE);

    EV_INFO << "Updated IPv4/UDP headers after CPRP payload change: delta=" << delta
            << ", oldIpTotal=" << oldIpTotalLength
            << ", newIpTotal=" << newIpTotalLength
            << ", oldUdpTotal=" << oldUdpTotalLength
            << ", newUdpTotal=" << newUdpTotalLength
            << ", udpChecksumReset=" << udpChecksumReset << endl;
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

    // 优先从 Header 提取路径
    auto pathHeader = getCpnPathHeader(packet);
    
    if (pathHeader != nullptr) {
        state.sidPath.clear();
        for (int i = 0; i < (int)pathHeader->getHopAddressArraySize(); i++) {
            state.sidPath.push_back(pathHeader->getHopAddress(i));
        }
    }
    else {
        // 后备方案：从 Tag 提取 (本地)
        auto pathTag = packet->findTag<CpnPathReq>();
        if (pathTag) {
            state.sidPath.clear();
            for (int i = 0; i < (int)pathTag->getHopAddressArraySize(); i++) {
                state.sidPath.push_back(pathTag->getHopAddress(i));
            }
        }
    }
}

Ptr<const CprpResponseMsg> CprpProcessorBase::getCprpResp(Packet *packet) {
    B offset = getCprpRespOffset(packet);
    if (offset < B(0)) return nullptr;

    auto chunk = packet->peekDataAt<Chunk>(offset, b(-1), OPTIONAL_CHUNK_PEEK_FLAGS);
    return dynamicPtrCast<const CprpResponseMsg>(chunk);
}

B CprpProcessorBase::getCprpRespOffset(Packet *packet) {
    if (!isCprpPacket(packet)) return B(-1);

    B offset = getPayloadOffset(packet);
    if (offset < B(0)) return B(-1);

    // 检查是否存在 CpnPathHeader，如果存在，RESP 消息在它之后
    auto pathHeader = getCpnPathHeader(packet);
    if (pathHeader != nullptr)
        offset += pathHeader->getChunkLength();

    if (b(offset) >= packet->getDataLength()) return B(-1);
    return offset;
}

Ptr<const CancelMsg> CprpProcessorBase::getCancelMsg(Packet *packet) {
    if (!isCprpPacket(packet)) return nullptr;

    auto ipv4Header = packet->peekAtFront<Ipv4Header>();
    if (!ipv4Header) return nullptr;
    
    auto ipLen = B(ipv4Header->getHeaderLength());

    if (ipv4Header->getProtocol() == &cprp::cprp) {
        // 形式1：网络层直接发送的原始IP CANCEL
        if (b(ipLen) >= packet->getDataLength()) return nullptr;
        auto chunk = packet->peekDataAt<Chunk>(ipLen, b(-1), OPTIONAL_CHUNK_PEEK_FLAGS);
        return dynamicPtrCast<const CancelMsg>(chunk);
    } 
    else if (ipv4Header->getProtocol() == &Protocol::udp) {
        // 形式2：应用层发送的UDP CANCEL
        B offset = ipLen + B(8); // 跳过 UDP 头
        
        // 同样检查路径头部
        auto pathHeader = getCpnPathHeader(packet);
        if (pathHeader != nullptr) {
            offset += pathHeader->getChunkLength();
        }
        if (b(offset) >= packet->getDataLength()) return nullptr;
        
        auto chunk = packet->peekDataAt<Chunk>(offset, b(-1), OPTIONAL_CHUNK_PEEK_FLAGS);
        return dynamicPtrCast<const CancelMsg>(chunk);
    }

    return nullptr;
}

std::vector<L3Address> CprpProcessorBase::getUpstreamNodes(const std::vector<L3Address>& sidPath) {
    std::vector<L3Address> upstream;
    if (sidPath.empty()) return upstream;

    // 获取所有本地接口的 IP 地址列表
    std::vector<L3Address> myAddresses;
    if (interfaceTable != nullptr) {
        for (int i = 0; i < interfaceTable->getNumInterfaces(); i++) {
            auto ie = interfaceTable->getInterface(i);
            if (!ie->isLoopback() && ie->getProtocolData<Ipv4InterfaceData>()) {
                myAddresses.push_back(L3Address(ie->getProtocolData<Ipv4InterfaceData>()->getIPAddress()));
            }
        }
    }

    int myIndex = -1;
    for (int i = 0; i < (int)sidPath.size(); i++) {
        for (const auto& addr : myAddresses) {
            if (sidPath[i] == addr) {
                myIndex = i;
                break;
            }
        }
        if (myIndex >= 0) break;
    }

    if (myIndex < 0) {
        EV_INFO << "getUpstreamNodes: Local address not found in SID path, treating all recorded hops as upstream." << endl;
        return sidPath;
    }

    // 上游节点是索引小于 myIndex 的节点
    for (int i = 0; i < myIndex; i++) {
        upstream.push_back(sidPath[i]);
    }

    EV_INFO << "getUpstreamNodes: Found local index " << myIndex << ", upstream nodes count: " << upstream.size() << endl;
    return upstream;
}

} // namespace inet
