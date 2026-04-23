package inet.distributed_computing_power_network.router;

import inet.common.contract.IModule;

simple BandwidthManager like IModule
{
    parameters:
        @display("i=block/routing");
        double defaultReservationTimeout @unit(s) = default(60s);  // 默认预留超时时间
        bool strictPruning = default(true);  // 严格剪枝模式：带宽不足时丢弃消息
        
    gates:
        input in;
        output out;
}
