#include "UserGatewayProcessor.h"

namespace inet {

Define_Module(UserGatewayProcessor);

INetfilter::IHook::Result UserGatewayProcessor::datagramLocalInHook(Packet *packet) {
    Enter_Method("datagramLocalInHook");
    if (!enabled) return ACCEPT;

    // 用户网关本地入站阶段仍处于IPv4解封装之前，不在这里修改IPv4/UDP报文内容。
    // CpnPathHeader在UDP payload交给UserGatewayApp后剥离，避免破坏IPv4后续decapsulate流程。
    EV_INFO << "UserGatewayProcessor: Accepting packet; path header stripping is deferred to UserGatewayApp." << endl;
    return ACCEPT;
}

} // namespace inet
