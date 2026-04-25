package inet.distributed_computing_power_network.router;

simple CprpProcessorBase
{
    parameters:
        @display("i=block/routing");
        bool enabled = default(true);
        bool strictPruning = default(true);
        string sessionManagerModule = default("^.sessionManager");
        string interfaceTableModule = default("^.interfaceTable");
        string ipv4Module = default("^.ipv4.ip");

    gates:
        input ipIn;
        output ipOut;
}
