//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 

package inet.distributed_computing_power_network.routing.common;

simple SessionManager
{
    parameters:
        @display("i=block/routing");
        double sessionTimeout @unit(s) = default(60s);
        double defaultReservationTimeout @unit(s) = default(60s);
        string interfaceTableModule = default("^.interfaceTable");
        double defaultInterfaceBandwidth @unit(bps) = default(100Mbps);
        bool strictPruning = default(true);
        
    gates:
        input in;
        output out;
}
