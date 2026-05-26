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
#include <queue>
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
    double maxNetworkBandwidth;
    double computeCost;

    std::queue<QueuedTask> taskQueue;
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
    void processTaskData(Packet *packet);
    void enqueueTask(const Ptr<const TaskDataMsg>& taskData);
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
