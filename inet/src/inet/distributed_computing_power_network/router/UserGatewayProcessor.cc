#include "UserGatewayProcessor.h"

namespace inet {

Define_Module(UserGatewayProcessor);

INetfilter::IHook::Result UserGatewayProcessor::datagramLocalInHook(Packet *packet) {
    Enter_Method("datagramLocalInHook");
    if (!enabled) return ACCEPT;

    // 1. 物理剥离路径 Header 并转换为本地 Tag (确保 UserGatewayApp 逻辑正常)
    stripPathHeader(packet);

    // 2. 策略调整：直接 ACCEPT 所有包
    // 用户网关网络层不负责撤销会话，拦截 CANCEL 会导致应用层收不到任务通知
    EV_INFO << "UserGatewayProcessor: Stripped path header and accepting packet to App layer." << endl;
    return ACCEPT;
}

} // namespace inet
