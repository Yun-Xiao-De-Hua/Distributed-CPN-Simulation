//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 

package inet.distributed_computing_power_network.node;

import inet.distributed_computing_power_network.applications.ComputeGatewayApp.ComputeGatewayApp;
import inet.distributed_computing_power_network.router.SessionManager;
import inet.distributed_computing_power_network.node.ComputeGatewayProcessor;
import inet.node.ospfv2.OspfRouter;

module ComputeGateway extends OspfRouter
{
    parameters:
        hasUdp = true;
        numApps = default(1);
        app[0].typename = "ComputeGatewayApp";
        int computeNodePort = default(5000);
        
    submodules:
        sessionManager: SessionManager {
            @display("p=300,100");
            interfaceTableModule = "^.interfaceTable";
        }
        
        computeGatewayProcessor: ComputeGatewayProcessor {
            @display("p=300,200");
            sessionManagerModule = "^.sessionManager";
            interfaceTableModule = "^.interfaceTable";
            ipv4Module = "^.ipv4";
            computeNodePort = computeNodePort;
        }
}