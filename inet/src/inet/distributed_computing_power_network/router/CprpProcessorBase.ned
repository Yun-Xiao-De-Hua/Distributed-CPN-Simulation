package inet.distributed_computing_power_network.router;

import inet.common.contract.IModule;

simple CprpProcessorBase like IModule
{
    parameters:
        @display("i=block/routing");
        bool enabled = default(true);
        bool strictPruning = default(true);
        string sessionManagerModule = default("^.sessionManager");
        string interfaceTableModule = default("^.interfaceTable");
        string ipv4Module = default("^.ipv4");
}
