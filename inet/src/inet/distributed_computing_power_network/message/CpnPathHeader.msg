import inet.common.INETDefs;
import inet.common.TagBase;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.L3Address;

namespace inet;

// 路径操作模式
enum CpnPathMode {
    PATH_RECORD_MODE = 0;   // 路径记录模式: 记录转发路径并进行带宽预留
    PATH_USE_MODE = 1;      // 路径使用模式: 按照SID列表进行源路由转发
}

// 算力网络路径Header (物理报文头，用于跨节点传输)
class CpnPathHeader extends inet::FieldsChunk
{
    int mode = 0;                    // 路径操作模式: PATH_RECORD_MODE 或 PATH_USE_MODE
    
    // 流标识
    int userId = -1;                 // 用户ID
    int taskId = -1;                 // 任务ID
    
    // 路径记录模式字段
    L3Address hopAddress[];          // 记录的跳地址列表(网络层填充)
    L3Address userGatewayAddress;    // 用户网关地址(用于带宽预留标识)
    double requiredBandwidth = 0;    // 所需带宽(用于预留)
    
    // 路径使用模式字段
    L3Address sidList[];             // SID路径列表(源路由)
    int currentHopIndex = 0;         // 当前跳索引
    
    chunkLength = B(64); // 基础长度；实际长度由处理器按数组规模显式更新
}

// 算力网络路径记录请求Tag (应用层 -> 网络层，本地指令)
class CpnPathReq extends TagBase
{
    int mode = 0;
    int userId = -1;
    int taskId = -1;
    L3Address hopAddress[];
    L3Address userGatewayAddress;
    double requiredBandwidth = 0;
    int reserveInterfaceId = -1;      // 本地预留接口ID，不写入物理路径Header
    L3Address sidList[];
    int currentHopIndex = 0;
}

// 算力网络路径记录指示Tag (网络层 -> 应用层)
class CpnPathInd extends TagBase
{
    int userId = -1;
    int taskId = -1;
    L3Address hopAddress[];
    int hopCount = 0;
    double reservedBandwidth = 0;
}
