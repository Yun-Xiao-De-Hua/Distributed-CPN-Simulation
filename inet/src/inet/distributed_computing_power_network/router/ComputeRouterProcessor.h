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

#ifndef INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_COMPUTEROUTERPROCESSOR_H_
#define INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_COMPUTEROUTERPROCESSOR_H_

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

class ComputeRouterProcessor : public cSimpleModule, public INetfilter::IHook
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
                                            const NetworkInterface *& outIE, L3Address& nextHopAddr) override;
    virtual Result datagramLocalInHook(Packet *packet, const NetworkInterface *inIE) override;
    virtual Result datagramLocalOutHook(Packet *packet) override;

    // RESP消息处理（核心路由选择算法）
    Result processCprpResp(Packet *packet, const NetworkInterface *inIE, 
                           const NetworkInterface *outIE);
    
    // 路由选择算法
    bool runRoutingSelection(RequestSessionState& existing, 
                            RequestSessionState& newResp,
                            int outInterfaceId);
    
    // Cancel消息处理
    void processCancelMsg(Packet *packet);
    
    // 发送Cancel消息
    void sendCancelMessages(const RequestSessionState& state);
    
    // 浅度包检测 - 刷新会话状态
    void refreshSessionIfMatch(Packet *packet);
    
    // 路径记录模式处理
    Result processPathRecordMode(Packet *packet, const NetworkInterface *outIE);
    
    // 源路由模式处理
    Result processPathUseMode(Packet *packet, const NetworkInterface *outIE);
    
    // 从RESP消息提取会话状态
    void extractSessionFromResp(RequestSessionState& state, Packet *packet);
    
    // 更新RESP消息字段
    void updateRespFields(Packet *packet, const RequestSessionState& state);

public:
    ComputeRouterProcessor();
    virtual ~ComputeRouterProcessor();
};

} // namespace inet

#endif /* INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_ROUTER_COMPUTEROUTERPROCESSOR_H_ */
