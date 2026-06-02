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
        double requestTimeout @unit(s) = default(5s);
        string interfaceTableModule = default("^.interfaceTable");

        // 按算力类型配置组播组地址 (每种算力类型只对应一个组播组地址)
        string cpuMulticastGroup = default("");   // CPU算力组播组地址
        string gpuMulticastGroup = default("");   // GPU算力组播组地址

        // 组播路由配置：组播地址 -> 转发接口列表
        // 格式: "组播地址 接口1,接口2,...;组播地址 接口1,..."
        // 例如: "225.0.1.1 ppp1;225.0.1.2 ppp1,ppp2"
        string multicastRoutes = default("");

    gates:
        input socketIn;
        output socketOut;
}
