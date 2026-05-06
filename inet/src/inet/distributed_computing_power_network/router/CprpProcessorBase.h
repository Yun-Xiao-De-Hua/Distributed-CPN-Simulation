#ifndef INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_CPRPPROCESSORBASE_H_
#define INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_CPRPPROCESSORBASE_H_

#include <omnetpp.h>
#include <vector>
#include "inet/common/ModuleAccess.h"
#include "inet/common/Protocol.h"
#include "inet/networklayer/contract/INetfilter.h"
#include "inet/networklayer/ipv4/Ipv4.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/InterfaceTable.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/common/InitStages.h"
#include "inet/distributed_computing_power_network/message/CpnPathHeader_m.h"
#include "inet/distributed_computing_power_network/message/cpn_message_m.h"
#include "inet/distributed_computing_power_network/router/SessionManager.h"

namespace inet {

class CprpProcessorBase : public cSimpleModule, public NetfilterBase::HookBase
{
protected:
    bool enabled;
    bool strictPruning;

    ModuleRefByPar<SessionManager> sessionManager;
    ModuleRefByPar<IInterfaceTable> interfaceTable;
    ModuleRefByPar<Ipv4> ipv4Module;

    struct PendingCancel {
        L3Address destAddr;
        int userId;
        int taskId;
        L3Address computeNodeAddress;
        int computeNodePort;
        int senderType;
    };
    std::vector<PendingCancel> pendingCancels;
    cMessage *sendCancelsMsg = nullptr;

    L3Address localAddress;

protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;

    virtual Result datagramPreRoutingHook(Packet *packet) override;
    virtual Result datagramForwardHook(Packet *packet) override;
    virtual Result datagramPostRoutingHook(Packet *packet) override;
    virtual Result datagramLocalInHook(Packet *packet) override;
    virtual Result datagramLocalOutHook(Packet *packet) override;

    virtual Result processCprpResp(Packet *packet);
    virtual Result processCancelMsg(Packet *packet);

    virtual bool shouldKeepNewSession(const RequestSessionState& existing,
                                       const RequestSessionState& newResp,
                                       int outInterfaceId);

    virtual void sendPendingCancels();
    virtual void sendCancelPacket(const PendingCancel& info);

    virtual void refreshSessionIfMatch(Packet *packet);

    virtual Result processPathRecordMode(Packet *packet);
    virtual Result processPathUseMode(Packet *packet);

    virtual void handlePathHeader(Packet *packet);
    virtual void stripPathHeader(Packet *packet);
    virtual B getPayloadOffset(Packet *packet);

    virtual Ptr<const CprpResponseMsg> getCprpResp(Packet *packet);
    virtual Ptr<const CancelMsg> getCancelMsg(Packet *packet);

    // 识别数据包是否属于 CPRP 协议流
    virtual bool isCprpPacket(Packet *packet);

    // 安全提取路径头
    virtual Ptr<const CpnPathHeader> getCpnPathHeader(Packet *packet);

    virtual void extractSessionFromResp(RequestSessionState& state, Packet *packet);

    virtual std::vector<L3Address> getUpstreamNodes(const std::vector<L3Address>& sidPath);

    virtual L3Address getLocalAddress();

public:
    CprpProcessorBase();
    virtual ~CprpProcessorBase();
};

} // namespace inet

#endif
