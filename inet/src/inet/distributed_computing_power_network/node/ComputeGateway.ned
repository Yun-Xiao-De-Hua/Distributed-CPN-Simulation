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

package inet.distributed_computing_power_network.node;
import inet.distributed_computing_power_network.applications.ComputeGatewayApp.ComputeGatewayApp;
import inet.distributed_computing_power_network.routing.ComputeGatewayProcessor;
import inet.distributed_computing_power_network.routing.common.SessionManager;
import inet.node.ospfv2.OspfRouter;

module ComputeGateway extends OspfRouter
{
    parameters:
        hasUdp = true;
        numApps = default(0);
        bool hasSessionManager = default(true);
        bool hasComputeGatewayProcessor = default(true);
        double sessionTimeout @unit(s) = default(60s);
        double defaultInterfaceBandwidth @unit(bps) = default(100Mbps);

    submodules:
        sessionManager: SessionManager if hasSessionManager {
            parameters:
                sessionTimeout = parent.sessionTimeout;
                interfaceTableModule = "^.interfaceTable";
                defaultInterfaceBandwidth = parent.defaultInterfaceBandwidth;
            @display("p=600,100");
        }
        computeGatewayProcessor: ComputeGatewayProcessor if hasComputeGatewayProcessor {
            parameters:
                sessionManagerModule = "^.sessionManager";
                interfaceTableModule = "^.interfaceTable";
                ipv4Module = "^.ipv4.ip";
            @display("p=600,150");
        }
        computeGatewayApp: ComputeGatewayApp {
            parameters:
                @display("p=375,75");
        }

    connections allowunconnected:
        computeGatewayProcessor.ipOut --> tn.in++ if hasComputeGatewayProcessor;
        computeGatewayProcessor.ipIn <-- tn.out++ if hasComputeGatewayProcessor;
        computeGatewayProcessor.cancelOut --> computeGatewayApp.cprpControlIn if hasComputeGatewayProcessor;
        computeGatewayApp.socketOut --> at.in++;
        computeGatewayApp.socketIn <-- at.out++;
}
