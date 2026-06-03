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

package inet.distributed_computing_power_network.applications.ComputeNodeApp;
import inet.applications.contract.IApp;

simple ComputeNodeApp like IApp
{
    parameters:
        int computeNodeId = default(0);                     // 算力节点编号
        int computeGatewayId = default(0);                  // 算力网关编号
        string localAddress = default("");
        int localPort = default(5000);
        int userNodePort = default(5000);
        string computeGatewayAddress = default("");
        int computeGatewayPort = default(5000);
        int computingType = default(0);                     // 算力类型，0 表示 CPU，1 表示 GPU
        double computingCapacity = default(1e10);           // 计算能力(FLOPs/s)
        double storageCapacity = default(0);                // 存储资源(MB)
        double computeNodeMaxBandwidth = default(100);          // 算力节点最大可用网络带宽(Mbps)
        double computeCost = default(0);                    // 使用成本(￥/s)
        string multicastAddress = default("");             // 算力节点组播组地址

    gates:
       input socketIn;
       output socketOut;
}
