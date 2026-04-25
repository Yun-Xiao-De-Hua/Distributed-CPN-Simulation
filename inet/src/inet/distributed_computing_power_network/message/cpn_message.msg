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

import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.L3Address;

namespace inet;

// ==============================================
// ================== CPRP 协议 ==================
// ==============================================

class CprpRequestMsg extends inet::FieldsChunk {
    // 通用字段
    int userId;           // 用户ID
    int taskId;           // 任务ID

    // REQ 专用字段
    string msgType = "CPRP_REQ";    
	L3Address userGatewayAddress;
 
 	// 任务数据参数
    simtime_t generationTime;   // 任务产生的时刻
    int computingType; 			// 任务所需算力类型，0表示CPU，1表示GPU
    double requiredStorage;     // 任务所需存储空间
    double computingAmount;     // 任务计算量
    double transferAmount;      // 任务传输量
    simtime_t totalDelayRequirement; // 总时延要求
    double budget;              // 预算
    double userMaxBandwidth;    // 用户最大传输带宽
    
    chunkLength = B(12 + 40); // 在构造函数中直接赋值
}

class CprpResponseMsg extends inet::FieldsChunk {
    // 通用字段
    int userId;           // 用户ID
    int taskId;           // 任务ID

    // RESP 专用字段
    string msgType = "CPRP_RESP";    
 
	L3Address computeNodeAddress;	// 算力节点地址 
	int computeNodeId;
	int computeNodePort;			// 算力节点端口号
    int computingType; 			// 节点算力类型，0表示CPU，1表示GPU    
    double computingCapacity;	// 算力
    double availableStorage;     // 可用存储    
    simtime_t sendTime;   // 发送时间戳
    
    // 资源预留参数
    double requiredBandwidth;       // 用户最大传输带宽
    simtime_t maxDelayTolerance;    // 最大时延忍耐值
    double computeCost;             // 任务计算费用
    
    // 时延信息
    simtime_t accumulatedDelay;     // 算力网关计算的总时延
    
    // 路由选择相关字段
    simtime_t lastHopSendTime;      // 上一跳发送时间戳
    L3Address lastHopAddress;       // 上一跳IP地址
    
    chunkLength = B(32 + 64);
}

class CprpConfirmMsg extends inet::FieldsChunk {
    // 通用字段
    int userId;           // 用户ID
    int taskId;           // 任务ID

    // Confirm 专用字段
    string msgType = "CPRP_CONFIRM";    
 
	int selectedNodeId;	// 最终选定的算力节点
	L3Address selectedNodeAddress;  // 选定算力节点IP
	int selectedNodePort;           // 选定算力节点端口
	int selectedPathIndex = 0;      // 用户选定的路径索引(可选)
	
	// 任务数据参数	
    simtime_t generationTime;   // 任务产生的时刻
    int computingType; 			// 任务所需算力类型，0表示CPU，1表示GPU
    double requiredStorage;     // 任务所需存储空间
    double computingAmount;     // 任务计算量
    double transferAmount;      // 任务传输量
    simtime_t totalDelayRequirement; // 总时延要求
    double budget;              // 预算 
    
    chunkLength = B(28 + 32);
}

// ==============================================
// ================= 用户节点相关 =================
// ==============================================

class TaskRequestMsg extends inet::FieldsChunk {
    // 通用字段
    int userId;           // 用户ID
    int taskId;           // 任务ID

    string msgType = "TaskRequestMsg";    
 
 	// 任务数据参数
    simtime_t generationTime;   // 任务产生的时刻
    int computingType; 			// 任务所需算力类型，0表示CPU，1表示GPU
    double requiredStorage;     // 任务所需存储空间
    double computingAmount;     // 任务计算量
    double transferAmount;      // 任务传输量
    simtime_t totalDelayRequirement; // 总时延要求
    double budget;              // 预算
    double userMaxBandwidth;    // 用户最大传输带宽
    
    chunkLength = B(12 + 40); // 在构造函数中直接赋值
}

// ==============================================
// ================= 用户网关相关 =================
// ==============================================

// 任务数据消息
class TaskDataMsg extends inet::FieldsChunk {
    // 通用字段
    int userId;           // 用户ID
    int taskId;           // 任务ID

    string msgType = "TaskDataMsg";    
	L3Address userNodeAddress;    
 
	// 任务数据参数	
    simtime_t generationTime;   // 任务产生的时刻
    int computingType; 			// 任务所需算力类型，0表示CPU，1表示GPU
    double requiredStorage;     // 任务所需存储空间
    double computingAmount;     // 任务计算量
    double transferAmount;      // 任务传输量
    simtime_t totalDelayRequirement; // 总时延要求
    double budget;              // 预算
    int priority = 5;           // DSCP优先级，默认EF(5)
    
    chunkLength = B(16 + 32); // 在构造函数中直接赋值
}

// 算力应答超时自消息
class RespTimeoutSelfMsg extends cMessage{
    string msgType = "RespTimeoutSelfMsg";
	int userId;
	int taskId;
}

// 算力应答汇总消息
struct computeNodeInfo{
    L3Address computeNodeAddress;	// 算力节点地址 
	int computeNodeId;	
    int computingType; 			// 节点算力类型，0表示CPU，1表示GPU    
    double computingCapacity;	// 算力
    double availableStorage;     // 可用存储    
    simtime_t sendTime;   // 发送时间戳   
}

class RespSummaryMsg extends inet::FieldsChunk {
    // 通用字段
    int userId;           // 用户ID
    int taskId;           // 任务ID

    string msgType = "RespSummaryMsg";    
 
	computeNodeInfo nodeInfo[];

    chunkLength = B(12 + 32); // 在构造函数中直接赋值
}

// ==============================================
// ================== CGMP 协议 ==================
// ==============================================

class CgmpQueryMsg extends inet::FieldsChunk {
    string msgType = "CGMP_Query";    
    
    chunkLength = B(1); // 在构造函数中直接赋值	    
}

class CgmpReportMsg extends inet::FieldsChunk {
    string msgType = "CGMP_Report";    
    
	int computeNodeId;	
    int computingType; 			// 节点算力类型，0表示CPU，1表示GPU    
    L3Address computeNodeAddress;
    double computingCapacity;	// 算力
    double availableStorage;     // 可用存储    
    simtime_t sendTime;   // 发送时间戳    
    
    chunkLength = B(8 + 16); // 在构造函数中直接赋值	    
}

// ==============================================
// ================== Cancel消息 =================
// ==============================================

enum CancelSenderType {
    SENDER_USER_GW = 0;
    SENDER_COMPUTE_GW = 1;
    SENDER_COMPUTE_ROUTER = 2;
}

class CancelMsg extends inet::FieldsChunk {
    int userId;                       // 用户ID
    int taskId;                       // 任务ID
    L3Address computeNodeAddress;     // 算力节点IP
    int computeNodePort;              // 算力节点端口
    int senderType;                   // 发送方类型 (CancelSenderType)
    
    string msgType = "CANCEL";
    chunkLength = B(20);
}

// ==============================================
// ============ 任务完成通告消息 =================
// ==============================================

class TaskCompletionMsg extends inet::FieldsChunk {
    int userId;                       // 用户ID
    int taskId;                       // 任务ID
    L3Address computeNodeAddress;     // 算力节点地址
    int computeNodePort;              // 算力节点端口
    L3Address userGatewayAddress;     // 用户网关地址
    simtime_t completionTime;         // 任务完成时间
    double taskResult;                // 任务结果大小
    
    string msgType = "TASK_COMPLETION";
    chunkLength = B(24);
}





