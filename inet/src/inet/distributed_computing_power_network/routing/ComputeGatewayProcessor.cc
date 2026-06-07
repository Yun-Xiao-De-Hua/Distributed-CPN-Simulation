//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 

#include "ComputeGatewayProcessor.h"
#include "inet/networklayer/ipv4/Ipv4Header_m.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"

namespace inet {

Define_Module(ComputeGatewayProcessor);

ComputeGatewayProcessor::ComputeGatewayProcessor() : CprpProcessorBase() {}
ComputeGatewayProcessor::~ComputeGatewayProcessor() {}

void ComputeGatewayProcessor::initialize(int stage) {
    CprpProcessorBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        computeNodePort = par("computeNodePort");
    }
}

void ComputeGatewayProcessor::extractSessionFromResp(RequestSessionState& state, Packet *packet) {
    CprpProcessorBase::extractSessionFromResp(state, packet);

    EV_INFO << "ComputeGatewayProcessor extracted session for task ("
            << state.userId << "," << state.taskId << ")" << endl;
}

INetfilter::IHook::Result ComputeGatewayProcessor::datagramPreRoutingHook(Packet *packet) {
    Enter_Method("datagramPreRoutingHook");
    Result result = CprpProcessorBase::datagramPreRoutingHook(packet);
    if (result != ACCEPT)
        return result;

    limitLocalCprpReqMulticast(packet);
    return ACCEPT;
}

void ComputeGatewayProcessor::limitLocalCprpReqMulticast(Packet *packet) {
    if (!enabled || strcmp(packet->getName(), "CPRP_REQ") != 0)
        return;

    auto ipv4Header = packet->peekAtFront<Ipv4Header>();
    if (ipv4Header == nullptr || ipv4Header->getProtocol() != &Protocol::udp)
        return;

    Ipv4Address destAddr = ipv4Header->getDestAddress();
    if (!destAddr.isMulticast())
        return;

    auto interfaceInd = packet->findTag<InterfaceInd>();
    if (interfaceInd == nullptr || interfaceTable == nullptr)
        return;

    auto ingressInterface = interfaceTable->getInterfaceById(interfaceInd->getInterfaceId());
    auto ingressIpv4Data = ingressInterface == nullptr ? nullptr : ingressInterface->getProtocolData<Ipv4InterfaceData>();
    if (ingressIpv4Data == nullptr || !ingressIpv4Data->isMemberOfMulticastGroup(destAddr))
        return;

    if (ipv4Header->getTimeToLive() <= 1)
        return;

    auto newIpv4Header = makeShared<Ipv4Header>(*ipv4Header);
    newIpv4Header->setTimeToLive(1);
    if (newIpv4Header->getCrcMode() == CRC_COMPUTED)
        newIpv4Header->updateCrc();

    packet->replaceDataAt(newIpv4Header, B(0), ipv4Header->getChunkLength(), Chunk::PF_ALLOW_INCOMPLETE);
    EV_INFO << "ComputeGatewayProcessor limited multicast CPRP_REQ TTL to 1 on ingress interface "
            << ingressInterface->getInterfaceName()
            << "; packet will be delivered locally and not forwarded to downstream multicast listeners." << endl;
}

INetfilter::IHook::Result ComputeGatewayProcessor::processCancelMsg(Packet *packet) {
    auto cancel = getCancelMsg(packet);
    auto ipv4Header = packet->peekAtFront<Ipv4Header>();
    bool isUdpCancel = ipv4Header != nullptr && ipv4Header->getProtocol() == &Protocol::udp;

    Result result = CprpProcessorBase::processCancelMsg(packet);

    if (cancel != nullptr) {
        Packet *appPacket = new Packet("CPRP_CANCEL");
        appPacket->insertAtBack(makeShared<CancelMsg>(*cancel));

        send(appPacket, "cancelOut");
        EV_INFO << "ComputeGatewayProcessor forwarded " << (isUdpCancel ? "UDP" : "raw")
                << " CPRP_CANCEL to ComputeGatewayApp control gate for task ("
                << cancel->getUserId() << "," << cancel->getTaskId() << ")" << endl;
    }

    return result;
}

void ComputeGatewayProcessor::handleSessionCreateFailure(const RequestSessionState& rejectedState) {
    CprpProcessorBase::handleSessionCreateFailure(rejectedState);

    auto cancel = makeShared<CancelMsg>();
    cancel->setUserId(rejectedState.userId);
    cancel->setTaskId(rejectedState.taskId);
    cancel->setComputeNodeAddress(rejectedState.computeNodeAddress);
    cancel->setComputeNodePort(rejectedState.computeNodePort);
    cancel->setSenderType(SENDER_COMPUTE_GW);
    cancel->setChunkLength(B(20));

    Packet *appPacket = new Packet("CPRP_CANCEL");
    appPacket->insertAtBack(cancel);
    send(appPacket, "cancelOut");

    EV_INFO << "ComputeGatewayProcessor notified ComputeGatewayApp to release queue soft state after session creation failure for task ("
            << rejectedState.userId << "," << rejectedState.taskId << ")" << endl;
}

INetfilter::IHook::Result ComputeGatewayProcessor::datagramPostRoutingHook(Packet *packet){
    Enter_Method("datagramPostRoutingHook");
    if (!enabled) return ACCEPT;

    if (!isCprpPacket(packet)) return ACCEPT;

    const char *pktName = packet->getName();
    Result result = ACCEPT;

    // 1. 处理路径头部封装 (针对本地发出的 UDP 包)
    handlePathHeader(packet);

    auto pathReqTag = packet->findTag<CpnPathReq>();
    if (pathReqTag != nullptr) {
        int mode = pathReqTag->getMode();
        if (mode == PATH_RECORD_MODE) {
            processPathRecordMode(packet);
        }
    }

    auto respProbe = getCprpResp(packet);
    if (strcmp(pktName, "CPRP_RESP") == 0 || respProbe != nullptr) {
        if (strcmp(pktName, "CPRP_RESP") != 0) {
            EV_INFO << "ComputeGatewayProcessor: packet name is '" << pktName
                    << "', but CprpResponseMsg was detected in payload; treating it as CPRP_RESP." << endl;
        }
        result = processCprpResp(packet);
        if (result == DROP) return DROP;
    }

    // 只处理本地发出的路径使用模式，物理Header的PATH_USE_MODE由预路由阶段消费，避免重复跳转。
    if (pathReqTag != nullptr) {
        int mode = pathReqTag->getMode();
        if (mode == PATH_USE_MODE) {
            processPathUseMode(packet);
        }
    }

    return result;
}

} // namespace inet
