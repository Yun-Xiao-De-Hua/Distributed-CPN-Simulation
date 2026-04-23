#ifndef INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_CPNPATHPROCESSOR_H_
#define INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_CPNPATHPROCESSOR_H_

#include <omnetpp.h>
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/contract/INetfilter.h"
#include "inet/networklayer/ipv4/Ipv4.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/InterfaceTable.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/networklayer/common/InterfaceTag_m.h"
#include "inet/distributed_computing_power_network/message/CpnPathHeader_m.h"
#include "inet/distributed_computing_power_network/router/BandwidthManager.h"

namespace inet {

class CpnPathProcessor : public cSimpleModule, public INetfilter::IHook
{
protected:
    bool enabled;
    bool strictPruning;
    
    ModuleRefByPar<BandwidthManager> bandwidthManager;
    ModuleRefByPar<IInterfaceTable> interfaceTable;
    ModuleRefByPar<Ipv4> ipv4Module;

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    
    virtual Result datagramPreRoutingHook(Packet *packet) override;
    virtual Result datagramForwardHook(Packet *packet) override;
    virtual Result datagramPostRoutingHook(Packet *packet, const NetworkInterface *inIE, 
                                            const NetworkInterface *& outIE, L3Address& nextHopAddr) override;
    virtual Result datagramLocalInHook(Packet *packet, const NetworkInterface *inIE) override;
    virtual Result datagramLocalOutHook(Packet *packet) override;

    Result processPathRecordMode(Packet *packet, const NetworkInterface *outIE);
    Result processPathUseMode(Packet *packet, const NetworkInterface *outIE);

public:
    CpnPathProcessor();
    virtual ~CpnPathProcessor();
};

} // namespace inet

#endif /* INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_CPNPATHPROCESSOR_H_ */
