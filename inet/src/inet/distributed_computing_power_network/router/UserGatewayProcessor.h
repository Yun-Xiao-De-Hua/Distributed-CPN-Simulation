#ifndef __INET_USERGATEWAYPROCESSOR_H_
#define __INET_USERGATEWAYPROCESSOR_H_

#include "inet/distributed_computing_power_network/router/CprpProcessorBase.h"

namespace inet {

class UserGatewayProcessor : public CprpProcessorBase
{
  protected:
    virtual Result datagramLocalInHook(Packet *packet) override;
};

} // namespace inet

#endif
