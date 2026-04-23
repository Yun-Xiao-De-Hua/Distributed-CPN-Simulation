package inet.distributed_computing_power_network.router;

import inet.common.contract.IModule;

simple CpnPathProcessor like IModule
{
    parameters:
        @display("i=block/routing");
        bool enabled = default(true);
        bool strictPruning = default(true);  // 严格剪枝：带宽不足时丢弃CPRP_RESP
        string bandwidthManagerModule = default("^.bandwidthManager");
        string interfaceTableModule = default("^.interfaceTable");
}
