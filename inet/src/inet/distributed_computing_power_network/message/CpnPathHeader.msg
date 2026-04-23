import inet.common.INETDefs;
import inet.common.TagBase;
import inet.networklayer.common.L3Address;

namespace inet;

// 路径操作模式
enum CpnPathMode {
    PATH_RECORD_MODE = 0;   // 路径记录模式: 记录转发路径并进行带宽预留
    PATH_USE_MODE = 1;      // 路径使用模式: 按照SID列表进行源路由转发
}

// 算力网络路径记录请求Tag (应用层 -> 网络层)
// 用于请求网络层记录路径或使用指定路径转发
class CpnPathReq extends TagBase
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
}

// 算力网络路径记录指示Tag (网络层 -> 应用层)
// 用于向应用层传递已记录的路径信息
class CpnPathInd extends TagBase
{
    int userId = -1;                 // 用户ID
    int taskId = -1;                 // 任务ID
    
    L3Address hopAddress[];          // 记录的跳地址列表
    int hopCount = 0;                // 跳数
    double reservedBandwidth = 0;    // 已预留带宽
}
