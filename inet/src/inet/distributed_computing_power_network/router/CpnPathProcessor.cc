#include "CpnPathProcessor.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"
#include "inet/networklayer/common/DscpTag_m.h"

namespace inet {

Define_Module(CpnPathProcessor);

CpnPathProcessor::CpnPathProcessor()
{
}

CpnPathProcessor::~CpnPathProcessor()
{
}

void CpnPathProcessor::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        enabled = par("enabled");
        strictPruning = par("strictPruning");
    }
    else if (stage == INITSTAGE_NETWORK_LAYER) {
        if (enabled) {
            ipv4Module.reference(this, "ipv4Module", true);
            if (ipv4Module.isValid()) {
                ipv4Module->registerHook(0, this);
                EV_INFO << "CpnPathProcessor registered as Netfilter hook" << endl;
            }
        }
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        bandwidthManager.reference(this, "bandwidthManagerModule", false);
        interfaceTable.reference(this, "interfaceTableModule", true);
        
        EV_INFO << "CpnPathProcessor initialized, enabled=" << enabled 
                << " strictPruning=" << strictPruning << endl;
    }
}

void CpnPathProcessor::handleMessage(cMessage *msg)
{
    delete msg;
}

INetfilter::IHook::Result CpnPathProcessor::datagramPreRoutingHook(Packet *packet)
{
    return ACCEPT;
}

INetfilter::IHook::Result CpnPathProcessor::datagramForwardHook(Packet *packet)
{
    return ACCEPT;
}

INetfilter::IHook::Result CpnPathProcessor::datagramPostRoutingHook(Packet *packet,
                                                                     const NetworkInterface *inIE,
                                                                     const NetworkInterface *& outIE,
                                                                     L3Address& nextHopAddr)
{
    if (!enabled) return ACCEPT;

    auto pathReq = packet->findTag<CpnPathReq>();
    if (pathReq == nullptr) return ACCEPT;

    int mode = pathReq->getMode();
    
    if (mode == PATH_RECORD_MODE) {
        return processPathRecordMode(packet, outIE);
    }
    else if (mode == PATH_USE_MODE) {
        return processPathUseMode(packet, outIE);
    }

    return ACCEPT;
}

INetfilter::IHook::Result CpnPathProcessor::datagramLocalInHook(Packet *packet, const NetworkInterface *inIE)
{
    return ACCEPT;
}

INetfilter::IHook::Result CpnPathProcessor::datagramLocalOutHook(Packet *packet)
{
    return ACCEPT;
}

INetfilter::IHook::Result CpnPathProcessor::processPathRecordMode(Packet *packet, const NetworkInterface *outIE)
{
    auto pathReq = packet->findTagForUpdate<CpnPathReq>();
    if (pathReq == nullptr || outIE == nullptr) return ACCEPT;

    int userId = pathReq->getUserId();
    int taskId = pathReq->getTaskId();
    double requiredBw = pathReq->getRequiredBandwidth();
    L3Address userGwAddr = pathReq->getUserGatewayAddress();

    Ipv4Address outAddr = outIE->getProtocolData<Ipv4InterfaceData>()->getIPAddress();
    
    int hopCount = pathReq->getHopAddressArraySize();
    pathReq->setHopAddressArraySize(hopCount + 1);
    pathReq->setHopAddress(hopCount, outAddr);
    
    EV_INFO << "Recording hop " << hopCount << ": " << outAddr 
            << " for flow (" << userId << "," << taskId << ")" << endl;

    if (bandwidthManager.isValid() && requiredBw > 0) {
        bool success = bandwidthManager->tryReserveBandwidth(
            outIE->getInterfaceId(),
            userId, taskId,
            userGwAddr,
            requiredBw,
            60.0
        );

        if (!success && strictPruning) {
            EV_INFO << "Strict pruning: dropping packet due to insufficient bandwidth" << endl;
            return DROP;
        }
    }

    return ACCEPT;
}

INetfilter::IHook::Result CpnPathProcessor::processPathUseMode(Packet *packet, const NetworkInterface *outIE)
{
    auto pathReq = packet->findTagForUpdate<CpnPathReq>();
    if (pathReq == nullptr) return ACCEPT;

    int currentIndex = pathReq->getCurrentHopIndex();
    int sidCount = pathReq->getSidListArraySize();

    if (currentIndex >= sidCount) {
        EV_INFO << "Reached end of SID list" << endl;
        return ACCEPT;
    }

    L3Address nextSid = pathReq->getSidList(currentIndex);
    pathReq->setCurrentHopIndex(currentIndex + 1);

    auto l3AddrReq = packet->addTagIfAbsent<L3AddressReq>();
    l3AddrReq->setDestAddress(nextSid);

    EV_INFO << "Source routing: forwarding to next SID " << nextSid 
            << " (hop " << (currentIndex + 1) << "/" << sidCount << ")" << endl;

    return ACCEPT;
}

} // namespace inet
