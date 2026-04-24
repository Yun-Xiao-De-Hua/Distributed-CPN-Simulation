//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "ComputeRouterProcessor.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"
#include "inet/networklayer/common/DscpTag_m.h"
#include "inet/distributed_computing_power_network/message/cpn_message_m.h"

namespace inet {

Define_Module(ComputeRouterProcessor);

ComputeRouterProcessor::ComputeRouterProcessor()
{
}

ComputeRouterProcessor::~ComputeRouterProcessor()
{
}

void ComputeRouterProcessor::initialize(int stage)
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
                EV_INFO << "ComputeRouterProcessor registered as Netfilter hook" << endl;
            }
        }
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        sessionManager.reference(this, "sessionManagerModule", false);
        interfaceTable.reference(this, "interfaceTableModule", true);
        
        EV_INFO << "ComputeRouterProcessor initialized, enabled=" << enabled 
                << " strictPruning=" << strictPruning << endl;
    }
}

void ComputeRouterProcessor::handleMessage(cMessage *msg)
{
    delete msg;
}

INetfilter::IHook::Result ComputeRouterProcessor::datagramPreRoutingHook(Packet *packet)
{
    if (!enabled) return ACCEPT;
    
    refreshSessionIfMatch(packet);
    
    return ACCEPT;
}

INetfilter::IHook::Result ComputeRouterProcessor::datagramForwardHook(Packet *packet)
{
    return ACCEPT;
}

INetfilter::IHook::Result ComputeRouterProcessor::datagramPostRoutingHook(Packet *packet,
                                                                          const NetworkInterface *inIE,
                                                                          const NetworkInterface *& outIE,
                                                                          L3Address& nextHopAddr)
{
    if (!enabled) return ACCEPT;

    const char *pktName = packet->getName();
    
    if (strcmp(pktName, "CPRP_RESP") == 0) {
        return processCprpResp(packet, inIE, outIE);
    }
    else if (strcmp(pktName, "CANCEL") == 0) {
        processCancelMsg(packet);
        return DROP;
    }
    
    auto pathReq = packet->findTag<CpnPathReq>();
    if (pathReq != nullptr) {
        int mode = pathReq->getMode();
        if (mode == PATH_RECORD_MODE) {
            return processPathRecordMode(packet, outIE);
        }
        else if (mode == PATH_USE_MODE) {
            return processPathUseMode(packet, outIE);
        }
    }

    return ACCEPT;
}

INetfilter::IHook::Result ComputeRouterProcessor::datagramLocalInHook(Packet *packet, const NetworkInterface *inIE)
{
    return ACCEPT;
}

INetfilter::IHook::Result ComputeRouterProcessor::datagramLocalOutHook(Packet *packet)
{
    return ACCEPT;
}

void ComputeRouterProcessor::refreshSessionIfMatch(Packet *packet)
{
    const char *pktName = packet->getName();
    
    int userId = -1, taskId = -1;
    
    if (strcmp(pktName, "CPRP_RESP") == 0) {
        auto resp = packet->peekAtFront<CprpResponseMsg>();
        if (resp) {
            userId = resp->getUserId();
            taskId = resp->getTaskId();
        }
    }
    else if (strcmp(pktName, "TASK_DATA") == 0) {
        auto data = packet->peekAtFront<TaskDataMsg>();
        if (data) {
            userId = data->getUserId();
            taskId = data->getTaskId();
        }
    }
    else if (strcmp(pktName, "CPRP_CONFIRM") == 0) {
        auto confirm = packet->peekAtFront<CprpConfirmMsg>();
        if (confirm) {
            userId = confirm->getUserId();
            taskId = confirm->getTaskId();
        }
    }
    
    if (userId >= 0 && taskId >= 0 && sessionManager.isValid()) {
        if (sessionManager->hasSession(userId, taskId)) {
            sessionManager->refreshSession(userId, taskId);
            EV_INFO << "Refreshed session for (" << userId << "," << taskId << ")" << endl;
        }
    }
}

INetfilter::IHook::Result ComputeRouterProcessor::processCprpResp(Packet *packet,
                                                                   const NetworkInterface *inIE,
                                                                   const NetworkInterface *outIE)
{
    if (!sessionManager.isValid() || !outIE) return ACCEPT;

    auto resp = packet->peekAtFront<CprpResponseMsg>();
    if (!resp) return ACCEPT;

    int userId = resp->getUserId();
    int taskId = resp->getTaskId();
    
    EV_INFO << "Processing CPRP_RESP for (" << userId << "," << taskId << ")" << endl;

    RequestSessionState newState;
    extractSessionFromResp(newState, packet);
    newState.interfaceId = outIE->getInterfaceId();
    
    if (!sessionManager->hasSession(userId, taskId)) {
        sessionManager->createSession(newState);
        EV_INFO << "First RESP for (" << userId << "," << taskId << "), forwarding" << endl;
        return ACCEPT;
    }
    
    RequestSessionState* existingState = sessionManager->getSessionForUpdate(userId, taskId);
    if (!existingState) return ACCEPT;

    bool shouldKeep = runRoutingSelection(*existingState, newState, outIE->getInterfaceId());
    
    if (shouldKeep) {
        EV_INFO << "New RESP is better, updating and forwarding" << endl;
        sessionManager->updateSession(newState);
        return ACCEPT;
    }
    else {
        EV_INFO << "Existing RESP is better, dropping and sending Cancel" << endl;
        sendCancelMessages(newState);
        return DROP;
    }
}

bool ComputeRouterProcessor::runRoutingSelection(RequestSessionState& existing,
                                                  RequestSessionState& newResp,
                                                  int outInterfaceId)
{
    simtime_t now = simTime();
    simtime_t networkDelay = now - newResp.lastHopSendTime;
    simtime_t roundTripDelay = networkDelay * 2;
    
    double newTotalDelay = newResp.totalDelay + roundTripDelay.dbl();
    
    EV_INFO << "Routing selection: networkDelay=" << networkDelay 
            << " roundTripDelay=" << roundTripDelay 
            << " newTotalDelay=" << newTotalDelay << endl;
    
    if (sessionManager.isValid()) {
        double availableBw = sessionManager->getAvailableBandwidth(outInterfaceId);
        if (availableBw < newResp.requiredBandwidth) {
            EV_INFO << "Bandwidth constraint not satisfied: required=" << newResp.requiredBandwidth
                    << " available=" << availableBw << endl;
            return false;
        }
    }
    
    newResp.totalDelay = newTotalDelay;
    newResp.minTotalDelay = newTotalDelay;
    
    if (existing.minTotalDelay < 0) {
        return true;
    }
    
    if (newTotalDelay < existing.minTotalDelay) {
        EV_INFO << "New delay " << newTotalDelay << " < existing " << existing.minTotalDelay << endl;
        return true;
    }
    
    return false;
}

void ComputeRouterProcessor::sendCancelMessages(const RequestSessionState& state)
{
    EV_INFO << "Sending Cancel messages for (" << state.userId << "," << state.taskId << ")" << endl;
    
    for (const auto& addr : state.sidPath) {
        auto cancel = makeShared<CancelMsg>();
        cancel->setUserId(state.userId);
        cancel->setTaskId(state.taskId);
        cancel->setComputeGatewayAddress(state.computeGatewayAddress);
        cancel->setComputeGatewayPort(state.computeGatewayPort);
        
        Packet *pkt = new Packet("CANCEL");
        pkt->insertAtBack(cancel);
        
        auto addrReq = pkt->addTagIfAbsent<L3AddressReq>();
        addrReq->setDestAddress(addr);
        
        ipv4Module->sendOut(pkt);
        
        EV_INFO << "Sent Cancel to " << addr << endl;
    }
}

void ComputeRouterProcessor::processCancelMsg(Packet *packet)
{
    auto cancel = packet->peekAtFront<CancelMsg>();
    if (!cancel) return;
    
    int userId = cancel->getUserId();
    int taskId = cancel->getTaskId();
    
    EV_INFO << "Processing Cancel for (" << userId << "," << taskId << ")" << endl;
    
    if (sessionManager.isValid() && sessionManager->hasSession(userId, taskId)) {
        sessionManager->removeSession(userId, taskId);
        EV_INFO << "Removed session for (" << userId << "," << taskId << ")" << endl;
    }
}

void ComputeRouterProcessor::extractSessionFromResp(RequestSessionState& state, Packet *packet)
{
    auto resp = packet->peekAtFront<CprpResponseMsg>();
    if (!resp) return;
    
    state.userId = resp->getUserId();
    state.taskId = resp->getTaskId();
    state.computeNodeAddress = resp->getComputeNodeAddress();
    state.computeNodeId = resp->getComputeNodeId();
    state.computeGatewayAddress = resp->getComputeGatewayAddress();
    state.computeGatewayPort = resp->getComputeGatewayPort();
    
    state.totalDelay = resp->getAccumulatedDelay().dbl() + 
                       resp->getComputingDelay().dbl() +
                       resp->getQueuingDelay().dbl() +
                       resp->getTransmissionDelay().dbl();
    state.minTotalDelay = -1;
    
    state.lastHopSendTime = resp->getLastHopSendTime();
    state.requiredBandwidth = resp->getRequiredBandwidth();
    state.userGatewayAddress = resp->getLastHopAddress();
    
    state.computingDelay = resp->getComputingDelay();
    state.queuingDelay = resp->getQueuingDelay();
    state.transmissionDelay = resp->getTransmissionDelay();
    state.computeCost = resp->getComputeCost();
    state.computingCapacity = resp->getComputingCapacity();
    state.availableStorage = resp->getAvailableStorage();
    
    auto pathReq = packet->findTag<CpnPathReq>();
    if (pathReq) {
        state.sidPath.clear();
        for (int i = 0; i < pathReq->getHopAddressArraySize(); i++) {
            state.sidPath.push_back(pathReq->getHopAddress(i));
        }
    }
}

INetfilter::IHook::Result ComputeRouterProcessor::processPathRecordMode(Packet *packet, const NetworkInterface *outIE)
{
    auto pathReq = packet->findTagForUpdate<CpnPathReq>();
    if (pathReq == nullptr || outIE == nullptr) return ACCEPT;

    Ipv4Address outAddr = outIE->getProtocolData<Ipv4InterfaceData>()->getIPAddress();
    
    int hopCount = pathReq->getHopAddressArraySize();
    pathReq->setHopAddressArraySize(hopCount + 1);
    pathReq->setHopAddress(hopCount, outAddr);
    
    EV_INFO << "Recording hop " << hopCount << ": " << outAddr << endl;

    return ACCEPT;
}

INetfilter::IHook::Result ComputeRouterProcessor::processPathUseMode(Packet *packet, const NetworkInterface *outIE)
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

    EV_INFO << "Source routing: forwarding to " << nextSid 
            << " (hop " << (currentIndex + 1) << "/" << sidCount << ")" << endl;

    return ACCEPT;
}

} // namespace inet
