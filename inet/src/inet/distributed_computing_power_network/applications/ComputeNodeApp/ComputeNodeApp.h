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

#ifndef INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_APPLICATIONS_COMPUTENODEAPP_COMPUTENODEAPP_H_
#define INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_APPLICATIONS_COMPUTENODEAPP_COMPUTENODEAPP_H_

#include <omnetpp.h>
#include <cstdint>
#include <map>
#include <queue>
#include <set>
#include <string>
#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/distributed_computing_power_network/message/cpn_message_m.h"

namespace inet {

class ComputeNodeApp: public ApplicationBase, public UdpSocket::ICallback  {
protected:
    struct QueuedTask {
        int userId = -1;
        int taskId = -1;
        L3Address userNodeAddress;
        int userNodePort = -1;
        simtime_t generationTime = SIMTIME_ZERO;
        int computingType = -1;
        double requiredStorage = 0;
        double computingAmount = 0;
        double transferAmount = 0;
        simtime_t totalDelayRequirement = SIMTIME_ZERO;
        double budget = 0;
        double userMaxBandwidth = 0;
        simtime_t executionStartTime = SIMTIME_ZERO;
    };

    struct TaskDataReceiveState {
        QueuedTask task;
        L3Address userGatewayAddress;
        int userGatewayPort = -1;
        int totalSegments = 0;
        int64_t totalTransferBytes = 0;
        int64_t receivedBytes = 0;
        std::set<int> receivedSegments;
        simtime_t firstSegmentReceiveTime = SIMTIME_ZERO;
        simtime_t lastSegmentReceiveTime = SIMTIME_ZERO;
    };

    int computeNodeId;
    int computeGatewayId;

    L3Address localAddress;
    int localPort;
    int userNodePort;
    L3Address computeGatewayAddress;
    int computeGatewayPort;
    L3Address multicastAddress;     // 组播地址，用于组播上报算力状态

    int computingType;
    double computingCapacity;
    double storageCapacity;
    double availableStorage;
    double computeNodeMaxBandwidth;
    double computeCost;

    std::queue<QueuedTask> taskQueue;
    std::map<std::pair<int, int>, TaskDataReceiveState> taskDataReceiveStates;
    QueuedTask activeTask;
    bool busy = false;
    cMessage *taskCompletionEvent = nullptr;

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

    // 发送组成员报告消息
    void sendCgmpReport(simtime_t querySendTime);
    simtime_t computeQueueingTime() const;
    simtime_t getTaskExecutionTime(const QueuedTask& task) const;
    simtime_t getTaskRemainingExecutionTime(const QueuedTask& task) const;
    void processTaskData(Packet *packet);
    QueuedTask makeQueuedTask(const Ptr<const TaskDataMsg>& taskData) const;
    void enqueueTask(const QueuedTask& task);
    void sendTaskDataTransferComplete(const Ptr<const TaskDataMsg>& taskData, const L3Address& userGatewayAddress, int userGatewayPort);
    void tryStartNextTask();
    bool validateTask(const QueuedTask& task, int& failureCode, std::string& failureReason) const;
    void startTaskExecution(const QueuedTask& task);
    void finishActiveTask();
    void sendTaskCompletion(const QueuedTask& task, bool success, int failureCode, const std::string& failureReason, double executionTime);
    void stripCpnPathHeader(Packet *packet);



public:
    ComputeNodeApp();
    virtual ~ComputeNodeApp();
};

} /* namespace inet */

#endif /* INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_APPLICATIONS_COMPUTENODEAPP_COMPUTENODEAPP_H_ */
