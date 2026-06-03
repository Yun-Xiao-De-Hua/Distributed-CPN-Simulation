import inet.common.INETDefs;
import inet.common.TagBase;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.L3Address;

namespace inet;

// CPN 路径处理模式
enum CpnPathMode {
    PATH_RECORD_MODE = 0;   // 路径记录模式，记录 RESP 返回路径并触发软状态预留
    PATH_USE_MODE = 1;      // 路径使用模式，按 SID 列表执行源路由转发
}

// CPN 路径 Header，封装在 UDP payload 中并随报文跨节点传输
class CpnPathHeader extends inet::FieldsChunk
{
    int mode = 0;                    // 路径处理模式，取值见 CpnPathMode
    
    // 流标识
    int userId = -1;                 // 用户 ID
    int taskId = -1;                 // 任务 ID
    
    // 路径记录模式字段
    L3Address hopAddress[];          // RESP 返回方向记录的逐跳出口地址列表，网络层填充
    L3Address userGatewayAddress;    // 用户网关地址，用于标识路径预留所属请求
    double requiredBandwidth = 0;    // 路径软状态预留带宽，单位: bps
    
    // 路径使用模式字段
    L3Address sidList[];             // 用户网关到算力节点方向的 SID 源路由列表
    int currentHopIndex = 0;         // 下一跳 SID 在 sidList 中的索引
    
    chunkLength = B(64);             // 基础序列化长度，实际长度由处理器按数组规模更新
}

// CPN 路径请求 Tag，应用层写入，网络层本地消费，不随报文跨节点传输
class CpnPathReq extends TagBase
{
    int mode = 0;                    // 路径处理模式，取值见 CpnPathMode
    int userId = -1;                 // 用户 ID
    int taskId = -1;                 // 任务 ID
    L3Address hopAddress[];          // RESP 返回方向记录的逐跳出口地址列表
    L3Address userGatewayAddress;    // 用户网关地址，用于标识路径预留所属请求
    double requiredBandwidth = 0;    // 路径软状态预留带宽，单位: bps
    L3Address sidList[];             // 用户网关到算力节点方向的 SID 源路由列表
    int currentHopIndex = 0;         // 下一跳 SID 在 sidList 中的索引
}

// CPN 路径指示 Tag，网络层写入，应用层本地消费，不随报文跨节点传输
class CpnPathInd extends TagBase
{
    int userId = -1;                 // 用户 ID
    int taskId = -1;                 // 任务 ID
    L3Address hopAddress[];          // RESP 返回方向记录的逐跳出口地址列表
    int hopCount = 0;                // hopAddress 中的有效跳数
    double reservedBandwidth = 0;    // 候选路径软状态预留带宽，单位: bps
}
