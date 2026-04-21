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
    
    chunkLength = B(12 + 32); // 在构造函数中直接赋值
}

class CprpResponseMsg extends inet::FieldsChunk {
    // 通用字段
    int userId;           // 用户ID
    int taskId;           // 任务ID

    // RESP 专用字段
    string msgType = "CPRP_RESP";    
 
	L3Address computeNodeAddress;	// 算力节点地址 
	int computeNodeId;	
    int computingType; 			// 节点算力类型，0表示CPU，1表示GPU    
    double computingCapacity;	// 算力
    double availableStorage;     // 可用存储    
    simtime_t sendTime;   // 发送时间戳    
    
    chunkLength = B(20 + 16); // 在构造函数中直接赋值
}

class CprpConfirmMsg extends inet::FieldsChunk {
    // 通用字段
    int userId;           // 用户ID
    int taskId;           // 任务ID

    // Confirm 专用字段
    string msgType = "CPRP_CONFIRM";    
 
	int selectedNodeId;	// 最终选定的算力节点
	
	// 任务数据参数	
    simtime_t generationTime;   // 任务产生的时刻
    int computingType; 			// 任务所需算力类型，0表示CPU，1表示GPU
    double requiredStorage;     // 任务所需存储空间
    double computingAmount;     // 任务计算量
    double transferAmount;      // 任务传输量
    simtime_t totalDelayRequirement; // 总时延要求
    double budget;              // 预算 
    
    chunkLength = B(20 + 32); // 在构造函数中直接赋值
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
    
    chunkLength = B(12 + 32); // 在构造函数中直接赋值
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
    
    chunkLength = B(12 + 32); // 在构造函数中直接赋值
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





