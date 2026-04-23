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

#ifndef INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_APPLICATIONS_USERNODEAPP_USERNODEAPP_H_
#define INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_APPLICATIONS_USERNODEAPP_USERNODEAPP_H_

#include <omnetpp.h>
#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/distributed_computing_power_network/message/cpn_message_m.h"

using namespace omnetpp;

namespace inet {

class UserNodeApp: public ApplicationBase, public UdpSocket::ICallback {
protected:
    int userNodeId;

    L3Address localAddress;
    int localPort;
    L3Address userGatewayAddress;
    int userGatewayPort;
    double maxTransmissionBandwidth;  // 用户最大传输带宽

    cMessage *selfTaskCreationEvent = nullptr;    // 自消息触发任务请求消息生成
    UdpSocket socket;

protected:
  virtual void initialize(int stage) override;
  virtual void handleMessageWhenUp(cMessage *msg) override;
  virtual void finish() override;
  virtual void handleStartOperation(LifecycleOperation *operation) override;
  virtual void handleStopOperation(LifecycleOperation *operation) override;
  virtual void handleCrashOperation(LifecycleOperation *operation) override;

  // UdpSocket::ICallback
  virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
  virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
  virtual void socketClosed(UdpSocket *socket) override;

  void sendTaskRequest();
  void sendCprpConfirm(Packet *packet);

public:
  UserNodeApp();
    virtual ~UserNodeApp();
};

} /* namespace inet */

#endif /* INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_APPLICATIONS_USERNODEAPP_USERNODEAPP_H_ */
