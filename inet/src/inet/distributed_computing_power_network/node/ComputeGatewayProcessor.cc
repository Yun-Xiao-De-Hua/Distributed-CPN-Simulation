//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 

#include "ComputeGatewayProcessor.h"
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

INetfilter::IHook::Result ComputeGatewayProcessor::datagramPostRoutingHook(Packet *packet){
    Enter_Method("datagramPostRoutingHook");
    if (!enabled) return ACCEPT;

    const char *pktName = packet->getName();
    Result result = ACCEPT;

    // 1. 处理路径头部封装 (针对本地发出的 UDP 包)
    handlePathHeader(packet);

    if (strcmp(pktName, "CPRP_RESP") == 0) {
        result = processCprpResp(packet);
        if (result == DROP) return DROP;
    }

    // 只处理路径使用模式，服务端IP和算力网关IP已在应用层写入SID列表
    auto pathReqTag = packet->findTag<CpnPathReq>();
    if (pathReqTag != nullptr) {
        int mode = pathReqTag->getMode();
        if (mode == PATH_USE_MODE) {
            processPathUseMode(packet);
        }
    }
    else {
        auto offset = getPayloadOffset(packet);
        if (offset >= B(0)) {
            auto pathHeader = packet->peekDataAt<CpnPathHeader>(offset);
            if (pathHeader != nullptr && pathHeader->getMode() == PATH_USE_MODE) {
                processPathUseMode(packet);
            }
        }
    }

    return result;
}

} // namespace inet
