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

package inet.distributed_computing_power_network.applications.UserNodeApp;
import inet.applications.contract.IApp;

simple UserNodeApp like IApp
{
	parameters:
       int userNodeId = default(0);			
       
       string localAddress = default("");					
       int localPort = default(5000);					
       string userGatewayAddress = default("");				
       int userGatewayPort = default(5000);					

	gates:	
	    input socketIn;
	    output socketOut;
}