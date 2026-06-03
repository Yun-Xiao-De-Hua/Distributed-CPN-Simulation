#ifndef INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_APPLICATIONS_COMPUTEGATEWAYAPP_COMPUTEGATEWAYAPP_H_
#define INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_APPLICATIONS_COMPUTEGATEWAYAPP_COMPUTEGATEWAYAPP_H_

#include <vector>
#include <map>
#include <string>
#include <unordered_map>
#include <omnetpp.h>
#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/distributed_computing_power_network/message/cpn_message_m.h"

namespace inet {

class ComputeGatewayApp: public ApplicationBase, public UdpSocket::ICallback {
protected:
    struct CIB{
        int nodeId;
        L3Address nodeAddress;
        int nodePort;
        int interfaceId;
        int computingType;
        L3Address serviceGroupAddress;
        double computingCapacity;
        double availableStorage;
        double computeNodeMaxBandwidth;
        double computeCost;
        simtime_t queueingTime;
        simtime_t querySendTime;
        simtime_t reportSendTime;
        double networkDelayMs;
        simtime_t updateTime;
    };

    struct CandidateEvaluation {
        CIB cib;
        double totalDelay = 0;
        bool eligible = false;
        std::string rejectReason;
    };

    struct ReservedTaskItem {
        int userId = -1;
        int taskId = -1;
        L3Address userNodeAddress;
        int userNodePort = -1;
        simtime_t remainingExecutionTime = SIMTIME_ZERO;
        bool reportedByComputeNode = false;
    };

    struct TaskQueueState {
        int queueId = -1;
        int computeNodeId = -1;
        L3Address serviceGroupAddress;
        L3Address computeNodeAddress;
        int computeNodePort = -1;
        simtime_t queueTotalTime = SIMTIME_ZERO;
        std::vector<ReservedTaskItem> taskQueue;
    };

    int computeGatewayId;

    int localPort;
    int userGatewayPort;
    int computeNodePort;
    L3Address localAddress;

    simtime_t cibUpdateInterval;
    std::unordered_map<int, std::unordered_map<int, CIB>> cibInfoMap;  // 存储算力组类型 -> (算力节点编号 -> 算力节点状态信息)的映射
    int nextQueueId = 1;
    std::map<std::pair<int, int>, TaskQueueState> taskQueueStateMap;
    std::vector<L3Address> computingPowerGroup; // 所管理算力组的组播地址
    std::map<L3Address, std::vector<int>> multicastRoutesMap;  // 组播地址 -> 转发接口ID列表

    void parseMulticastRoutes(const char *routesConfig);
    void logCurrentCib() const;
    void logTaskQueueStates() const;
    TaskQueueState& getOrCreateTaskQueueState(const CIB& cib);
    const TaskQueueState *findTaskQueueState(int computingType, int computeNodeId) const;
    simtime_t getReservedQueueingTime(int computingType, int computeNodeId) const;
    bool reserveTaskQueueItem(const CprpRequestMsg& requestInfo, const CandidateEvaluation& selectedCandidate);
    bool removeReservedTaskQueueItem(int userId, int taskId, const L3Address& computeNodeAddress, int computeNodePort, const char *reason);
    void reconcileTaskQueueWithReport(const CIB& cib, const Ptr<const CgmpReportMsg>& reportInfo);
    void processCprpCancel(Packet *packet);
    std::vector<CandidateEvaluation> evaluateCandidateNodes(const CprpRequestMsg& requestInfo, const std::unordered_map<int, CIB>& groupMap);
    bool selectBestCandidate(const std::vector<CandidateEvaluation>& candidates, CandidateEvaluation& bestCandidate) const;

    cMessage *SelfCibUpdateEvent = nullptr;    // CIB状态更新自消息
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

    // 发送组成员查询消息
    void sendCgmpQuery();
    // 更新CIB
    void updateCib(Packet *packet);
    // 发送算力应答
    void sendCprpResponse(Packet *packet);

public:
    ComputeGatewayApp();
    virtual ~ComputeGatewayApp();
};

} /* namespace inet */

#endif /* INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_APPLICATIONS_COMPUTEGATEWAYAPP_COMPUTEGATEWAYAPP_H_ */
