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

package inet.distributed_computing_power_network.applications.UserGatewayApp;
import inet.applications.contract.IApp;

simple UserGatewayApp like IApp
{
	parameters:
	    int userGatewayId;
	   
        int localPort = default(5000);            
    	int userNodePort = default(5000);  
    	int computeGatewayPort = default(5000);  
    	int computeNodePort = default(5000);  
        string localAddress = default("");		
    	string multicastInterfaces = default("");  // 组播转发接口列表，空格分隔，如 "ppp0 ppp1"
    	string interfaceTableModule = default("^.interfaceTable");  // 接口表模块路径
    	
    	// 按算力类型配置组播组地址
    	string cpuMulticastGroups = default("");   // CPU算力组播组地址列表，空格分隔
    	string gpuMulticastGroups = default("");   // GPU算力组播组地址列表，空格分隔

    gates:
        input socketIn;
        output socketOut;
}