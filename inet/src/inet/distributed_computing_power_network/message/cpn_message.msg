import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.L3Address;

namespace inet;

// CPRP 协议消息

class CprpRequestMsg extends inet::FieldsChunk {
    // 任务标识
    int userId;                       // 用户 ID
    int taskId;                       // 任务 ID

    // 请求来源
    string msgType = "CPRP_REQ";     // 消息类型
    L3Address userGatewayAddress;    // 发起 CPRP_REQ 的用户网关地址
    L3Address userNodeAddress;       // 产生任务的用户节点地址
    int userNodePort;                // 用户节点接收端口
 
    // 任务资源需求
    simtime_t generationTime;        // 任务生成时间，单位: s
    int computingType;               // 任务所需算力类型，0 表示 CPU，1 表示 GPU
    double requiredStorage;          // 任务所需存储空间，单位: MB
    double computingAmount;          // 任务计算量，单位: FLOPs
    double transferAmount;           // 任务数据量，单位: MB
    simtime_t totalDelayRequirement; // 端到端最大时延容忍值，单位: s
    double budget;                   // 任务预算
    double userMaxBandwidth;         // 用户请求的最大传输带宽，单位: Mbps
    
    chunkLength = B(32 + 40);       // 固定序列化长度
}

class CprpResponseMsg extends inet::FieldsChunk {
    // 任务标识
    int userId;                       // 用户 ID
    int taskId;                       // 任务 ID

    // 候选算力节点信息
    string msgType = "CPRP_RESP";    // 消息类型
    L3Address computeNodeAddress;    // 算力节点地址
    int computeNodeId;               // 算力节点 ID
    int computeNodePort;             // 算力节点接收端口
    int computingType;               // 算力节点类型，0 表示 CPU，1 表示 GPU
    double computingCapacity;        // 算力节点计算能力，单位: FLOPs/s
    double availableStorage;         // 算力节点当前可用存储，单位: MB
    double computeNodeMaxBandwidth;  // 算力节点最大可用网络带宽，单位: Mbps
    simtime_t sendTime;              // CPRP_RESP 生成时间戳，单位: s
 
    // 资源约束与预留
    double requiredBandwidth;       // 路径软状态预留带宽，单位: bps
    simtime_t maxDelayTolerance;    // 端到端最大时延容忍值，单位: s
    double computeCost;             // 算力节点使用成本，单位: CNY/s
    
    // 时延信息
    simtime_t accumulatedDelay;     // RESP 返回路径动态累计时延，不含用户节点到用户网关 RTT，单位: s
    
    // 路由选择相关字段
    simtime_t lastHopSendTime;      // 上一跳发送该 RESP 的时间戳，单位: s
    L3Address lastHopAddress;       // 上一跳出口 IP 地址
    
    chunkLength = B(32 + 72);
}

class CprpConfirmMsg extends inet::FieldsChunk {
    // 任务标识
    int userId;                       // 用户 ID
    int taskId;                       // 任务 ID

    // 用户节点确认的候选路径
    string msgType = "CPRP_CONFIRM"; // 消息类型
    int selectedNodeId;              // 用户最终选定的算力节点 ID
    L3Address selectedNodeAddress;   // 用户最终选定的算力节点地址
    int selectedNodePort;            // 用户最终选定的算力节点端口
    int selectedPathIndex = 0;       // 用户在 RESP_SUMMARY 中选定的候选路径索引
 
    chunkLength = B(28);
}

enum CancelSenderType {
    SENDER_USER_GW = 0;        // CANCEL 由用户网关发起
    SENDER_COMPUTE_GW = 1;     // CANCEL 由算力网关发起
    SENDER_COMPUTE_ROUTER = 2; // CANCEL 由算力路由器发起
}

class CancelMsg extends inet::FieldsChunk {
    int userId;                       // 用户 ID
    int taskId;                       // 任务 ID
    L3Address computeNodeAddress;     // 需要撤销软状态的算力节点地址
    int computeNodePort;              // 需要撤销软状态的算力节点端口
    int senderType;                   // CANCEL 发送方类型，取值见 CancelSenderType
    
    string msgType = "CPRP_CANCEL";  // 消息类型
    chunkLength = B(20);
}

// 用户节点请求消息

class TaskRequestMsg extends inet::FieldsChunk {
    // 任务标识
    int userId;                       // 用户 ID
    int taskId;                       // 任务 ID

    string msgType = "TASK_REQUEST"; // 消息类型

    // 任务资源需求
    simtime_t generationTime;        // 任务生成时间，单位: s
    int computingType;               // 任务所需算力类型，0 表示 CPU，1 表示 GPU
    double requiredStorage;          // 任务所需存储空间，单位: MB
    double computingAmount;          // 任务计算量，单位: FLOPs
    double transferAmount;           // 任务数据量，单位: MB
    simtime_t totalDelayRequirement; // 端到端最大时延容忍值，单位: s
    double budget;                   // 任务预算
    double userMaxBandwidth;         // 用户请求的最大传输带宽，单位: Mbps

    chunkLength = B(12 + 40);       // 固定序列化长度
}

// 用户网关相关消息

class TaskDataMsg extends inet::FieldsChunk {
    // 任务标识
    int userId;                       // 用户 ID
    int taskId;                       // 任务 ID

    // 任务来源
    string msgType = "TASK_DATA";    // 消息类型
    L3Address userNodeAddress;       // 产生任务的用户节点地址
    int userNodePort;                // 用户节点接收端口

    // 任务资源需求
    simtime_t generationTime;        // 任务生成时间，单位: s
    int computingType;               // 任务所需算力类型，0 表示 CPU，1 表示 GPU
    double requiredStorage;          // 任务所需存储空间，单位: MB
    double computingAmount;          // 任务计算量，单位: FLOPs
    double transferAmount;           // 任务数据量，单位: MB
    simtime_t totalDelayRequirement; // 端到端最大时延容忍值，单位: s
    double budget;                   // 任务预算
    double userMaxBandwidth;         // 用户请求的最大传输带宽，单位: Mbps
    int priority = 5;                // DSCP 优先级，默认值 5
    int segmentIndex = 0;            // TASK_DATA 分片序号，从 0 开始
    int totalSegments = 1;           // TASK_DATA 总分片数
    double totalTransferBytes = 0;   // 任务数据总字节数，按 transferAmount 换算
    double segmentPayloadBytes = 0;  // 当前分片承载的真实负载字节数
    
    chunkLength = B(20 + 40 + 24);  // 固定序列化长度，不含 ByteCountChunk 真实负载
}

class RespTimeoutSelfMsg extends cMessage {
    string msgType = "RESP_TIMEOUT_SELF"; // 用户网关本地超时自消息类型，不经网络发送
    int userId;                           // 用户 ID
    int taskId;                           // 任务 ID
}

struct computeNodeInfo {
    L3Address computeNodeAddress;   // 候选算力节点地址
    int computeNodeId;              // 候选算力节点 ID
    int computeNodePort;            // 候选算力节点接收端口
    int computingType;              // 候选算力节点类型，0 表示 CPU，1 表示 GPU
    double computingCapacity;       // 候选算力节点计算能力，单位: FLOPs/s
    double availableStorage;        // 候选算力节点当前可用存储，单位: MB
    double computeNodeMaxBandwidth; // 候选算力节点最大可用网络带宽，单位: Mbps
    double computeCost;             // 候选算力节点使用成本，单位: CNY/s
    simtime_t sendTime;             // 候选信息生成时间戳，单位: s
}

struct routeInfo {
    string sidPath;              // 用户网关到算力节点方向的源路由路径
    simtime_t totalDelay;        // 候选路径总时延，不含用户节点到用户网关 RTT，单位: s
    double reservedBandwidth;    // 候选路径软状态预留带宽，单位: bps
}

struct computeCandidateInfo {
    computeNodeInfo nodeInfo;     // 候选算力节点信息
    routeInfo pathInfo;           // 候选路径信息
}

class RespSummaryMsg extends inet::FieldsChunk {
    // 任务标识
    int userId;                       // 用户 ID
    int taskId;                       // 任务 ID

    string msgType = "RESP_SUMMARY"; // 消息类型
    simtime_t sendTime;              // 用户网关发送候选摘要的时间戳，用于用户节点估算接入 RTT，单位: s
 
    computeCandidateInfo candidateInfo[]; // 用户网关收集到的候选算力路径列表

    chunkLength = B(12 + 32);       // 基础序列化长度，不含候选数组扩展部分
}

// CGMP 协议消息

class CgmpQueryMsg extends inet::FieldsChunk {
    string msgType = "CGMP_QUERY";  // 消息类型
    simtime_t sendTime;             // 算力网关发送查询的时间戳，单位: s
    
    chunkLength = B(8);             // 固定序列化长度
}

struct cgmpTaskState {
    int userId;                       // 用户 ID
    int taskId;                       // 任务 ID
    simtime_t remainingExecutionTime; // 任务剩余执行时间，单位: s
}

class CgmpReportMsg extends inet::FieldsChunk {
    string msgType = "CGMP_REPORT"; // 消息类型
    
    // 算力节点状态
    int computeNodeId;             // 算力节点 ID
    int computeNodePort;           // 算力节点接收端口
    int computingType;             // 算力节点类型，0 表示 CPU，1 表示 GPU
    L3Address computeNodeAddress;  // 算力节点地址
    L3Address serviceGroupAddress; // 算力节点所属算力组播地址
    double computingCapacity;      // 算力节点计算能力，单位: FLOPs/s
    double availableStorage;       // 算力节点当前可用存储，单位: MB
    double computeNodeMaxBandwidth; // 算力节点最大可用网络带宽，单位: Mbps
    double computeCost;            // 算力节点使用成本，单位: CNY/s
    simtime_t queueingTime;        // 算力节点上报的预计排队时间，单位: s
    cgmpTaskState taskState[];     // 算力节点当前任务队列状态
    simtime_t querySendTime;       // 对应 CGMP_QUERY 的发送时间戳，单位: s
    simtime_t sendTime;            // CGMP_REPORT 发送时间戳，单位: s
    
    chunkLength = B(12 + 72);     // 基础序列化长度，不含任务状态数组扩展部分
}


// 任务完成通告消息

enum TaskFailureCode {
    TASK_OK = 0;                       // 任务成功完成
    TASK_INVALID_COMPUTING_TYPE = 1;   // 算力类型不匹配
    TASK_INSUFFICIENT_STORAGE = 2;     // 算力节点存储资源不足
    TASK_INSUFFICIENT_BANDWIDTH = 3;   // 算力节点网络带宽不足
    TASK_INVALID_RESOURCE_REQUEST = 4; // 任务资源请求参数非法
}

class TaskCompletionMsg extends inet::FieldsChunk {
    int userId;                       // 用户 ID
    int taskId;                       // 任务 ID
    L3Address computeNodeAddress;     // 算力节点地址
    int computeNodePort;              // 算力节点接收端口
    L3Address userNodeAddress;        // 用户节点地址
    simtime_t completionTime;         // 任务完成时间，单位: s
    bool success;                     // 任务是否成功完成
    int failureCode = 0;              // 失败原因码，取值见 TaskFailureCode
    string failureReason;             // 失败原因文本
    double taskResult;                // 任务结果大小，单位: MB
    double executionTime;             // 实际执行耗时，单位: s
    
    string msgType = "TASK_COMPLETION"; // 消息类型
    chunkLength = B(64);
}

class TaskDataTransferCompleteMsg extends inet::FieldsChunk {
    int userId;                       // 用户 ID
    int taskId;                       // 任务 ID
    L3Address computeNodeAddress;     // 已接收 TASK_DATA 的算力节点地址
    int computeNodePort;              // 算力节点接收端口
    L3Address userGatewayAddress;     // 接收传输完成通告的用户网关地址
    simtime_t receiveTime;            // 算力节点收到 TASK_DATA 的时间，单位: s

    string msgType = "TASK_DATA_TRANSFER_COMPLETE"; // 消息类型，表示 TASK_DATA 已到达算力节点，不表示任务计算完成
    chunkLength = B(56);
}





