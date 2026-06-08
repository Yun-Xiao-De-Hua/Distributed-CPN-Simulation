#include "inet/networklayer/common/L3AddressResolver.h"

#include <fstream>
#include <set>
#include <sstream>
#include <string>

#include "UserNodeApp.h"
#include "json.hpp"

Define_Module(inet::UserNodeApp);

namespace inet {

namespace {

using json = nlohmann::json;

const json emptyJsonObject = json::object();

const json *findJsonValue(const json& task, const json& userDefaults, const json& globalDefaults, const char *fieldName)
{
    if (task.contains(fieldName))
        return &task.at(fieldName);
    if (userDefaults.is_object() && userDefaults.contains(fieldName))
        return &userDefaults.at(fieldName);
    if (globalDefaults.is_object() && globalDefaults.contains(fieldName))
        return &globalDefaults.at(fieldName);
    return nullptr;
}

double readJsonDouble(const json& task, const json& userDefaults, const json& globalDefaults, const char *fieldName, const std::string& context)
{
    const json *value = findJsonValue(task, userDefaults, globalDefaults, fieldName);
    if (value == nullptr)
        throw cRuntimeError("Missing numeric field '%s' in %s", fieldName, context.c_str());
    if (!value->is_number())
        throw cRuntimeError("Field '%s' must be numeric in %s", fieldName, context.c_str());
    return value->get<double>();
}

int readJsonInt(const json& task, const json& userDefaults, const json& globalDefaults, const char *fieldName, const std::string& context)
{
    const json *value = findJsonValue(task, userDefaults, globalDefaults, fieldName);
    if (value == nullptr)
        throw cRuntimeError("Missing integer field '%s' in %s", fieldName, context.c_str());
    if (!value->is_number_integer())
        throw cRuntimeError("Field '%s' must be an integer in %s", fieldName, context.c_str());
    return value->get<int>();
}

std::string buildTaskContext(const std::string& fileName, int userId, int taskIndex)
{
    std::ostringstream stream;
    stream << "task file '" << fileName << "', userId=" << userId << ", taskIndex=" << taskIndex;
    return stream.str();
}

} // namespace

UserNodeApp::UserNodeApp() {
    // TODO Auto-generated constructor stub

}

UserNodeApp::~UserNodeApp() {
    for (auto event : taskCreationEvents)
        cancelAndDelete(event);
    taskCreationEvents.clear();
    taskTimerIndexByMessage.clear();
}

void UserNodeApp::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
       this->userNodeId = par("userNodeId");
       this->localPort = par("localPort");
       this->localAddress = L3AddressResolver().resolve(par("localAddress"));
       this->userGatewayAddress = L3AddressResolver().resolve(par("userGatewayAddress"));
       this->userGatewayPort = par("userGatewayPort");
       this->maxTransmissionBandwidth = par("maxTransmissionBandwidth");
       this->taskFile = par("taskFile").stdstringValue();

       loadTaskSpecs();
    }

    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        socket.setOutputGate(gate("socketOut"));
        socket.bind(localAddress,localPort);
        socket.setCallback(this);   // 将当前应用实例注册为 socket 的回调处理对象

        scheduleTaskRequests();
    }
}

void UserNodeApp::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        auto timerIt = taskTimerIndexByMessage.find(msg);
        if (timerIt != taskTimerIndexByMessage.end()) {
            int taskIndex = timerIt->second;
            taskTimerIndexByMessage.erase(timerIt);
            if (taskIndex >= 0 && taskIndex < (int)taskCreationEvents.size() && taskCreationEvents[taskIndex] == msg)
                taskCreationEvents[taskIndex] = nullptr;

            if (taskIndex < 0 || taskIndex >= (int)pendingTasks.size()) {
                EV_WARN << "Invalid TASK_REQUEST timer index=" << taskIndex << ", message=" << msg->getName() << endl;
                delete msg;
                return;
            }

            EV_INFO << "Received TaskCreationSelfMsg for task(" << userNodeId << "," << pendingTasks[taskIndex].taskId
                    << ") at " << simTime() << std::endl;
            sendTaskRequest(pendingTasks[taskIndex]);
            delete msg;
        }
        else {
            EV_WARN << "UserNodeApp received unknown self message: " << msg->getName() << endl;
            delete msg;
        }
    }
    else {
        // 处理底层传上来的 UDP 数据
        socket.processMessage(msg);
    }
}

void UserNodeApp::loadTaskSpecs()
{
    pendingTasks.clear();
    if (taskFile.empty())
        throw cRuntimeError("UserNodeApp userId=%d requires taskFile; task parameters must be loaded from JSON", userNodeId);

    loadTaskSpecsFromJson(taskFile.c_str());

    std::set<int> taskIds;
    for (const auto& task : pendingTasks) {
        if (!taskIds.insert(task.taskId).second)
            throw cRuntimeError("Duplicate taskId=%d for userId=%d", task.taskId, userNodeId);
    }

    EV_INFO << "User" << userNodeId << " loaded " << pendingTasks.size()
            << " task(s) from JSON task file " << taskFile << endl;
}

void UserNodeApp::loadTaskSpecsFromJson(const char *fileName)
{
    std::string resolvedPath = resolveResourcePath(fileName);
    std::ifstream input(resolvedPath.c_str());
    if (!input.is_open())
        throw cRuntimeError("Cannot open user task file '%s' resolved as '%s'", fileName, resolvedPath.c_str());

    json root;
    try {
        input >> root;
    }
    catch (const std::exception& e) {
        throw cRuntimeError("Cannot parse user task file '%s': %s", resolvedPath.c_str(), e.what());
    }

    if (!root.is_object())
        throw cRuntimeError("User task file '%s' must contain a top-level JSON object", resolvedPath.c_str());

    const json& globalDefaults = root.contains("defaults") ? root.at("defaults") : emptyJsonObject;
    if (!globalDefaults.is_object())
        throw cRuntimeError("Field 'defaults' must be an object in user task file '%s'", resolvedPath.c_str());

    if (root.contains("users")) {
        const json& users = root.at("users");
        if (!users.is_array())
            throw cRuntimeError("Field 'users' must be an array in user task file '%s'", resolvedPath.c_str());

        bool foundUser = false;
        for (int userIndex = 0; userIndex < (int)users.size(); userIndex++) {
            const json& user = users.at(userIndex);
            if (!user.is_object())
                throw cRuntimeError("Each users[] entry must be an object in user task file '%s'", resolvedPath.c_str());

            std::string userContext = "user entry in task file '" + resolvedPath + "'";
            int fileUserId = readJsonInt(user, emptyJsonObject, emptyJsonObject, "userId", userContext);
            if (fileUserId != userNodeId)
                continue;
            if (foundUser)
                throw cRuntimeError("Duplicate users[] entry for userId=%d in task file '%s'", userNodeId, resolvedPath.c_str());
            foundUser = true;

            const json& userDefaults = user.contains("defaults") ? user.at("defaults") : emptyJsonObject;
            if (!userDefaults.is_object())
                throw cRuntimeError("Field 'defaults' for userId=%d must be an object in task file '%s'", userNodeId, resolvedPath.c_str());

            if (!user.contains("tasks") || !user.at("tasks").is_array())
                throw cRuntimeError("Field 'tasks' for userId=%d must be an array in task file '%s'", userNodeId, resolvedPath.c_str());

            const json& tasks = user.at("tasks");
            for (int taskIndex = 0; taskIndex < (int)tasks.size(); taskIndex++) {
                const json& taskJson = tasks.at(taskIndex);
                if (!taskJson.is_object())
                    throw cRuntimeError("Each task for userId=%d must be an object in task file '%s'", userNodeId, resolvedPath.c_str());

                std::string context = buildTaskContext(resolvedPath, userNodeId, taskIndex);
                UserTaskSpec task;
                task.taskId = readJsonInt(taskJson, userDefaults, globalDefaults, "taskId", context);
                task.requestTime = SimTime(readJsonDouble(taskJson, userDefaults, globalDefaults, "requestTime", context));
                task.computingType = readJsonInt(taskJson, userDefaults, globalDefaults, "computingType", context);
                task.requiredStorage = readJsonDouble(taskJson, userDefaults, globalDefaults, "requiredStorage", context);
                task.dataSize = readJsonDouble(taskJson, userDefaults, globalDefaults, "dataSize", context);
                task.computingAmount = readJsonDouble(taskJson, userDefaults, globalDefaults, "computingAmount", context);
                task.delayTolerance = SimTime(readJsonDouble(taskJson, userDefaults, globalDefaults, "delayTolerance", context));
                task.budget = readJsonDouble(taskJson, userDefaults, globalDefaults, "budget", context);
                validateTaskSpec(task, context.c_str());
                pendingTasks.push_back(task);
            }
        }
        if (!foundUser)
            EV_INFO << "Task file '" << resolvedPath << "' contains no tasks for userId=" << userNodeId << endl;
    }
    else if (root.contains("tasks")) {
        const json& tasks = root.at("tasks");
        if (!tasks.is_array())
            throw cRuntimeError("Field 'tasks' must be an array in user task file '%s'", resolvedPath.c_str());

        for (int taskIndex = 0; taskIndex < (int)tasks.size(); taskIndex++) {
            const json& taskJson = tasks.at(taskIndex);
            if (!taskJson.is_object())
                throw cRuntimeError("Each tasks[] entry must be an object in user task file '%s'", resolvedPath.c_str());
            if (taskJson.contains("userId") && readJsonInt(taskJson, emptyJsonObject, emptyJsonObject, "userId", resolvedPath) != userNodeId)
                continue;

            std::string context = buildTaskContext(resolvedPath, userNodeId, taskIndex);
            UserTaskSpec task;
            task.taskId = readJsonInt(taskJson, emptyJsonObject, globalDefaults, "taskId", context);
            task.requestTime = SimTime(readJsonDouble(taskJson, emptyJsonObject, globalDefaults, "requestTime", context));
            task.computingType = readJsonInt(taskJson, emptyJsonObject, globalDefaults, "computingType", context);
            task.requiredStorage = readJsonDouble(taskJson, emptyJsonObject, globalDefaults, "requiredStorage", context);
            task.dataSize = readJsonDouble(taskJson, emptyJsonObject, globalDefaults, "dataSize", context);
            task.computingAmount = readJsonDouble(taskJson, emptyJsonObject, globalDefaults, "computingAmount", context);
            task.delayTolerance = SimTime(readJsonDouble(taskJson, emptyJsonObject, globalDefaults, "delayTolerance", context));
            task.budget = readJsonDouble(taskJson, emptyJsonObject, globalDefaults, "budget", context);
            validateTaskSpec(task, context.c_str());
            pendingTasks.push_back(task);
        }
    }
    else {
        throw cRuntimeError("User task file '%s' must contain either 'users' or 'tasks'", resolvedPath.c_str());
    }
}

void UserNodeApp::validateTaskSpec(const UserTaskSpec& task, const char *source) const
{
    if (task.taskId <= 0)
        throw cRuntimeError("Invalid taskId=%d in %s", task.taskId, source);
    if (task.requestTime < SIMTIME_ZERO)
        throw cRuntimeError("Invalid requestTime=%s for taskId=%d in %s", task.requestTime.str().c_str(), task.taskId, source);
    if (task.computingType != 0 && task.computingType != 1)
        throw cRuntimeError("Invalid computingType=%d for taskId=%d in %s", task.computingType, task.taskId, source);
    if (task.requiredStorage < 0)
        throw cRuntimeError("Invalid requiredStorage=%g for taskId=%d in %s", task.requiredStorage, task.taskId, source);
    if (task.dataSize <= 0)
        throw cRuntimeError("Invalid dataSize=%g for taskId=%d in %s", task.dataSize, task.taskId, source);
    if (task.computingAmount <= 0)
        throw cRuntimeError("Invalid computingAmount=%g for taskId=%d in %s", task.computingAmount, task.taskId, source);
    if (task.delayTolerance <= SIMTIME_ZERO)
        throw cRuntimeError("Invalid delayTolerance=%s for taskId=%d in %s", task.delayTolerance.str().c_str(), task.taskId, source);
    if (task.budget < 0)
        throw cRuntimeError("Invalid budget=%g for taskId=%d in %s", task.budget, task.taskId, source);
}

void UserNodeApp::scheduleTaskRequests()
{
    taskCreationEvents.assign(pendingTasks.size(), nullptr);
    for (int i = 0; i < (int)pendingTasks.size(); i++) {
        const auto& task = pendingTasks[i];
        std::ostringstream name;
        name << "TaskCreationSelfMsg-" << task.taskId;
        auto event = new cMessage(name.str().c_str());
        taskCreationEvents[i] = event;
        taskTimerIndexByMessage[event] = i;
        scheduleAt(task.requestTime, event);
        EV_INFO << "User" << userNodeId << " scheduled TASK_REQUEST for task(" << userNodeId << "," << task.taskId
                << ") at " << task.requestTime << ", dataSize=" << task.dataSize
                << " MB, computingType=" << task.computingType << endl;
    }
}

void UserNodeApp::sendTaskRequest(const UserTaskSpec& task)
{
    EV_INFO << "User" << userNodeId << " is to send TASK_REQUEST for task(" << userNodeId << "," << task.taskId << ")...\n";

    auto payload = makeShared<TaskRequestMsg>();
    payload->setUserId(this->userNodeId);
    payload->setTaskId(task.taskId);
    payload->setGenerationTime(simTime());
    payload->setComputingType(task.computingType);
    payload->setRequiredStorage(task.requiredStorage);
    payload->setComputingAmount(task.computingAmount);
    payload->setTransferAmount(task.dataSize);
    payload->setTotalDelayRequirement(task.delayTolerance);
    payload->setBudget(task.budget);
    payload->setUserMaxBandwidth(this->maxTransmissionBandwidth / 1e6);

    UserTaskContext taskContext;
    taskContext.generationTime = payload->getGenerationTime();
    taskContext.computingType = payload->getComputingType();
    taskContext.requiredStorage = payload->getRequiredStorage();
    taskContext.computingAmount = payload->getComputingAmount();
    taskContext.transferAmount = payload->getTransferAmount();
    taskContext.totalDelayRequirement = payload->getTotalDelayRequirement();
    taskContext.budget = payload->getBudget();
    taskContext.userMaxBandwidth = payload->getUserMaxBandwidth();
    taskContextCache[payload->getTaskId()] = taskContext;

    std::string messageType = payload->getMsgType();
    Packet *pkt = new Packet(messageType.c_str());
    pkt->insertAtBack(payload);

    socket.sendTo(pkt, userGatewayAddress, userGatewayPort);

    EV_INFO << "User" << this->userNodeId << " has sent TASK_REQUEST for task(" << this->userNodeId
            << "-" << payload->getTaskId() << "), generationTime=" << payload->getGenerationTime()
            << ", transferAmount=" << payload->getTransferAmount()
            << " MB, userMaxBandwidth=" << payload->getUserMaxBandwidth() << " Mbps\n";
}

void UserNodeApp::sendCprpConfirm(Packet *packet)
{
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(packet) << std::endl;

    // 提取任务请求负载信息
    const auto& sumInfo = packet->popAtFront<RespSummaryMsg>();

    if(sumInfo == nullptr){
        EV_WARN << "Error: Received a Packet named '" << packet->getName()
                 << "', but it does not contain a RESP_SUMMARY payload. Discarding.";
        delete packet;
        return;
    }

    if (sumInfo->getCandidateInfoArraySize() == 0) {
        EV_WARN << "RESP_SUMMARY contains no candidate information." << std::endl;
        delete packet;
        return;
    }

    auto taskIt = taskContextCache.find(sumInfo->getTaskId());
    if (taskIt == taskContextCache.end()) {
        EV_WARN << "No cached task context found for taskId=" << sumInfo->getTaskId() << std::endl;
        delete packet;
        return;
    }

    simtime_t userAccessOneWayDelay = simTime() - sumInfo->getSendTime();
    if (userAccessOneWayDelay < SIMTIME_ZERO) {
        EV_WARN << "RESP_SUMMARY sendTime is later than UserNode receive time; user access RTT is clamped to 0." << endl;
        userAccessOneWayDelay = SIMTIME_ZERO;
    }
    simtime_t userAccessRtt = userAccessOneWayDelay * 2;

    int selectedPathIndex = 0;
    const computeCandidateInfo *selectedCandidate = nullptr;
    simtime_t selectedTotalDelay = SIMTIME_ZERO;
    for (int i = 0; i < (int)sumInfo->getCandidateInfoArraySize(); i++) {
        const auto& candidate = sumInfo->getCandidateInfo(i);
        simtime_t candidateTotalDelay = candidate.pathInfo.totalDelay + userAccessRtt;

        EV_INFO << "User" << userNodeId << " candidate[" << i << "] delay evaluation: "
                << "gatewayToComputeTotalDelay=" << candidate.pathInfo.totalDelay
                << ", userAccessRtt=" << userAccessRtt
                << ", totalDelay=" << candidateTotalDelay
                << ", delayTolerance=" << taskIt->second.totalDelayRequirement
                << ", eligible=" << (candidateTotalDelay < taskIt->second.totalDelayRequirement ? "true" : "false") << endl;

        if (candidateTotalDelay >= taskIt->second.totalDelayRequirement)
            continue;

        if (selectedCandidate == nullptr ||
            candidateTotalDelay < selectedTotalDelay ||
            (candidateTotalDelay == selectedTotalDelay && candidate.nodeInfo.computeCost < selectedCandidate->nodeInfo.computeCost))
        {
            selectedCandidate = &candidate;
            selectedPathIndex = i;
            selectedTotalDelay = candidateTotalDelay;
        }
    }

    if (selectedCandidate == nullptr) {
        EV_WARN << "RESP_SUMMARY candidate selection failed: no candidate satisfies end-to-end delay tolerance." << std::endl;
        delete packet;
        return;
    }

    const auto& selectedNode = selectedCandidate->nodeInfo;

    EV_INFO << "User" << userNodeId << " selected CPRP route from RESP_SUMMARY: selectedPathIndex="
            << selectedPathIndex
            << ", nodeId="
            << selectedNode.computeNodeId
            << ", nodeAddress=" << selectedNode.computeNodeAddress
            << ", nodePort=" << selectedNode.computeNodePort
            << ", computeCost=" << selectedNode.computeCost
            << ", gatewayToComputeTotalDelay=" << selectedCandidate->pathInfo.totalDelay
            << ", userAccessRtt=" << userAccessRtt
            << ", totalDelay=" << selectedTotalDelay
            << ", reservedBandwidth=" << selectedCandidate->pathInfo.reservedBandwidth
            << ", sidPath=[" << selectedCandidate->pathInfo.sidPath << "]" << endl;

    // CPRP_CONFIRM 只通告最终选定的节点和路径索引，任务参数由用户网关侧缓存恢复。
    auto payload = makeShared<CprpConfirmMsg>();
    payload->setUserId(sumInfo->getUserId());
    payload->setTaskId(sumInfo->getTaskId());
    payload->setSelectedNodeId(selectedNode.computeNodeId);
    payload->setSelectedNodeAddress(selectedNode.computeNodeAddress);
    payload->setSelectedNodePort(selectedNode.computeNodePort);
    payload->setSelectedPathIndex(selectedPathIndex);

    std::string messageType = payload->getMsgType();
    Packet *pkt = new Packet(messageType.c_str());
    pkt->insertAtBack(payload);

    socket.sendTo(pkt, userGatewayAddress, userGatewayPort);

    EV_INFO << "User node has sent CPRP_CONFIRM for task(" << sumInfo->getUserId()
            << "," << sumInfo->getTaskId() << ") using selectedNodeId="
            << selectedNode.computeNodeId
            << ", selectedNodeAddress=" << selectedNode.computeNodeAddress
            << ", selectedNodePort=" << selectedNode.computeNodePort
            << ", selectedPathIndex=" << selectedPathIndex << "\n";

    delete packet;
}

void UserNodeApp::processTaskCompletion(Packet *packet)
{
    const auto& completion = packet->popAtFront<TaskCompletionMsg>();
    if (completion == nullptr) {
        EV_WARN << "Error: Received a Packet named '" << packet->getName()
                << "', but it does not contain a TaskCompletionMsg chunk. Discarding.";
        delete packet;
        return;
    }

    int taskId = completion->getTaskId();
    if (completion->getSuccess()) {
        EV_INFO << "User" << userNodeId << " received successful TASK_COMPLETION for task("
                << completion->getUserId() << "," << taskId << ") from "
                << completion->getComputeNodeAddress()
                << ", executionTime=" << completion->getExecutionTime()
                << ", resultSize=" << completion->getTaskResult() << " MB" << endl;
    }
    else {
        EV_WARN << "User" << userNodeId << " received failed TASK_COMPLETION for task("
                << completion->getUserId() << "," << taskId << ") from "
                << completion->getComputeNodeAddress()
                << ", failureCode=" << completion->getFailureCode()
                << ", reason=" << completion->getFailureReason() << endl;
    }

    taskContextCache.erase(taskId);
    delete packet;
}

// UdpSocket::ICallback
void UserNodeApp::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    if(strcmp(packet->getName(), "RESP_SUMMARY") == 0){
        sendCprpConfirm(packet);
    }
    else if(strcmp(packet->getName(), "TASK_COMPLETION") == 0){
        processTaskCompletion(packet);
    }
    else{
        EV_WARN << "Unknown packet type: " << packet->getName() << endl;
        delete packet;
    }
}

void UserNodeApp::socketErrorArrived(UdpSocket *socket, Indication *indication)
{

}

void UserNodeApp::socketClosed(UdpSocket *socket)
{

}

void UserNodeApp::finish()
{

}

void UserNodeApp::handleStartOperation(LifecycleOperation *operation)
{
}

void UserNodeApp::handleStopOperation(LifecycleOperation *operation)
{
    socket.close();
}

void UserNodeApp::handleCrashOperation(LifecycleOperation *operation)
{
    socket.destroy();
}

} /* namespace inet */
