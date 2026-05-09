#ifndef INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTING_COMPUTEROUTERPROCESSOR_H_
#define INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTING_COMPUTEROUTERPROCESSOR_H_

#include "inet/distributed_computing_power_network/routing/common/CprpProcessorBase.h"

namespace inet {

class ComputeRouterProcessor : public CprpProcessorBase
{
public:
    ComputeRouterProcessor();
    virtual ~ComputeRouterProcessor();
};

} // namespace inet

#endif
