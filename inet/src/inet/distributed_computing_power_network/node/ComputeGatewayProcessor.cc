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

} // namespace inet
