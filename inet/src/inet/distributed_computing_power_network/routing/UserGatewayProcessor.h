#ifndef __INET_USERGATEWAYPROCESSOR_H_
#define __INET_USERGATEWAYPROCESSOR_H_

#include "inet/distributed_computing_power_network/routing/common/CprpProcessorBase.h"

namespace inet {

class UserGatewayProcessor : public CprpProcessorBase
{
  protected:
    virtual Result datagramPostRoutingHook(Packet *packet) override;
    virtual Result datagramLocalInHook(Packet *packet) override;
    virtual Result processLocalCprpResp(Packet *packet);
    virtual void cleanupCancelSession(Packet *packet);
};

} // namespace inet

#endif
