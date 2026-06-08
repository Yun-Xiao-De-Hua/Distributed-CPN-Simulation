#ifndef INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_APPLICATIONS_USERNODEAPP_USERNODEAPP_H_
#define INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_APPLICATIONS_USERNODEAPP_USERNODEAPP_H_

#include <omnetpp.h>
#include <map>
#include <string>
#include <vector>
#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/distributed_computing_power_network/message/cpn_message_m.h"

using namespace omnetpp;

namespace inet {

struct UserTaskContext {
    simtime_t generationTime;
    int computingType;
    double requiredStorage;
    double computingAmount;
    double transferAmount;
    simtime_t totalDelayRequirement;
    double budget;
    double userMaxBandwidth;
};

struct UserTaskSpec {
    int taskId = -1;
    simtime_t requestTime = SIMTIME_ZERO;
    int computingType = 0;
    double requiredStorage = 0;
    double dataSize = 0;
    double computingAmount = 0;
    simtime_t delayTolerance = SIMTIME_ZERO;
    double budget = 0;
};

class UserNodeApp: public ApplicationBase, public UdpSocket::ICallback {
protected:
    int userNodeId;

    L3Address localAddress;
    int localPort;
    L3Address userGatewayAddress;
    int userGatewayPort;
    double maxTransmissionBandwidth;  // 用户最大传输带宽
    std::string taskFile;

    std::vector<UserTaskSpec> pendingTasks;
    std::map<int, UserTaskContext> taskContextCache;

    std::vector<cMessage *> taskCreationEvents;    // 自消息触发任务请求消息生成
    std::map<cMessage *, int> taskTimerIndexByMessage;
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

  void loadTaskSpecs();
  void loadTaskSpecsFromJson(const char *fileName);
  void validateTaskSpec(const UserTaskSpec& task, const char *source) const;
  void scheduleTaskRequests();
  void sendTaskRequest(const UserTaskSpec& task);
  void sendCprpConfirm(Packet *packet);
  void processTaskCompletion(Packet *packet);

public:
  UserNodeApp();
    virtual ~UserNodeApp();
};

} /* namespace inet */

#endif /* INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_APPLICATIONS_USERNODEAPP_USERNODEAPP_H_ */

