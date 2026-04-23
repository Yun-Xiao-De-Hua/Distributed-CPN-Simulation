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
import inet.node.ospfv2.OspfRouter;
import inet.distributed_computing_power_network.router.BandwidthManager;
import inet.distributed_computing_power_network.router.CpnPathProcessor;

module Router extends OspfRouter
{
    parameters:
        bool hasBandwidthManager = default(true);
        bool hasCpnPathProcessor = default(true);
        @display("i=abstract/router");
        
    submodules:
        bandwidthManager: BandwidthManager if hasBandwidthManager {
            @display("p=600,100");
        }
        cpnPathProcessor: CpnPathProcessor if hasCpnPathProcessor {
            parameters:
                ipv4Module = "^.ipv4";
            @display("p=600,150");
        }
}