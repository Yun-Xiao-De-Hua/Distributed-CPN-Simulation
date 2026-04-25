#ifndef INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_CPRPPROCESSORBASE_H_
#define INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_CPRPPROCESSORBASE_H_

#include <omnetpp.h>
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/contract/INetfilter.h"
#include "inet/networklayer/ipv4/Ipv4.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/InterfaceTable.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/networklayer/common/InterfaceTag_m.h"
#include "inet/distributed_computing_power_network/message/CpnPathHeader_m.h"
#include "inet/distributed_computing_power_network/router/SessionManager.h"

namespace inet {

class CprpProcessorBase : public cSimpleModule, public INetfilter::IHook
{
protected:
    bool enabled;
    bool strictPruning;
    
    ModuleRefByPar<SessionManager> sessionManager;
    ModuleRefByPar<IInterfaceTable> interfaceTable;
    ModuleRefByPar<Ipv4> ipv4Module;

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    
    virtual Result datagramPreRoutingHook(Packet *packet) override;
    virtual Result datagramForwardHook(Packet *packet) override;
    virtual Result datagramPostRoutingHook(Packet *packet, const NetworkInterface *inIE, 
                                            const NetworkInterface *& outIE, 
                                            L3Address& nextHopAddr) override;
    virtual Result datagramLocalInHook(Packet *packet, const NetworkInterface *inIE) override;
    virtual Result datagramLocalOutHook(Packet *packet) override;

    virtual Result processCprpResp(Packet *packet, const NetworkInterface *inIE, 
                                    const NetworkInterface *outIE);
    
    virtual bool shouldKeepNewSession(const RequestSessionState& existing,
                                       const RequestSessionState& newResp,
                                       int outInterfaceId);
    
    virtual void processCancelMsg(Packet *packet);
    
    virtual void sendCancelMessages(const RequestSessionState& state, 
                                     CancelSenderType senderType);
    
    virtual void refreshSessionIfMatch(Packet *packet);
    
    virtual Result processPathRecordMode(Packet *packet, const NetworkInterface *outIE);
    virtual Result processPathUseMode(Packet *packet, const NetworkInterface *outIE);
    
    virtual void extractSessionFromResp(RequestSessionState& state, Packet *packet);

public:
    CprpProcessorBase();
    virtual ~CprpProcessorBase();
};

} // namespace inet

#endif
