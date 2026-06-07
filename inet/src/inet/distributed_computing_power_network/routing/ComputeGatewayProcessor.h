//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 

#ifndef INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTING_COMPUTEGATEWAYPROCESSOR_H_
#define INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTING_COMPUTEGATEWAYPROCESSOR_H_

#include "inet/distributed_computing_power_network/routing/common/CprpProcessorBase.h"

namespace inet {

class ComputeGatewayProcessor : public CprpProcessorBase
{
protected:
    int computeNodePort;

protected:
    virtual void initialize(int stage) override;

    virtual Result datagramPreRoutingHook(Packet *packet) override;
    virtual void extractSessionFromResp(RequestSessionState& state, Packet *packet) override;
    virtual Result processCancelMsg(Packet *packet) override;
    virtual Result datagramPostRoutingHook(Packet *packet) override;
    virtual void handleSessionCreateFailure(const RequestSessionState& rejectedState) override;

    virtual void limitLocalCprpReqMulticast(Packet *packet);

public:
    ComputeGatewayProcessor();
    virtual ~ComputeGatewayProcessor();
};

} // namespace inet

#endif
