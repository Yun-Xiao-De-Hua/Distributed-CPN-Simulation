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

package inet.distributed_computing_power_network.applications.ComputeGatewayApp;
import inet.applications.contract.IApp;

simple ComputeGatewayApp like IApp
{
	parameters:
	    int computeGatewayId;
	    
        int localPort = default(5000);   
        string localAddress = default("");         
    	int userGatewayPort = default(5000);  
    	int computeNodePort = default(5000);  
    	string interfaceTableModule = default("^.interfaceTable");
    	
    	double cibUpdateInterval = default(0.5);
    	
    	string multicastGroups = default("");
    	string groupAddress = default("");
    	string multicastRoutes = default("");

    gates:
        input socketIn;
        output socketOut;
}