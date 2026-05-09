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
import inet.distributed_computing_power_network.applications.UserGatewayApp.UserGatewayApp;
import inet.distributed_computing_power_network.routing.UserGatewayProcessor;
import inet.node.ospfv2.OspfRouter;

module UserGateway extends OspfRouter
{
    parameters:
        hasUdp = true;        
        numApps = default(1);
        app[0].typename = "UserGatewayApp";
        bool hasUserGatewayProcessor = default(true);

    submodules:
        userGatewayProcessor: UserGatewayProcessor if hasUserGatewayProcessor {
            parameters:
                sessionManagerModule = "";
                interfaceTableModule = "^.interfaceTable";
                ipv4Module = "^.ipv4.ip";
            @display("p=600,150");
        }

    connections allowunconnected:
        userGatewayProcessor.ipOut --> tn.in++ if hasUserGatewayProcessor;
        userGatewayProcessor.ipIn <-- tn.out++ if hasUserGatewayProcessor;
}
