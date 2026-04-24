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
import inet.distributed_computing_power_network.router.SessionManager;
import inet.distributed_computing_power_network.router.ComputeRouterProcessor;

// 算力路由器模块
// 支持请求会话软状态管理、算力路由选择算法、浅度包检测
module ComputeRouter extends OspfRouter
{
    parameters:
        bool hasSessionManager = default(true);
        bool hasComputeRouterProcessor = default(true);
        double sessionTimeout @unit(s) = default(60s);  // 请求会话超时时间
        @display("i=abstract/router");
        
    submodules:
        sessionManager: SessionManager if hasSessionManager {
            @display("p=600,100");
        }
        computeRouterProcessor: ComputeRouterProcessor if hasComputeRouterProcessor {
            parameters:
                ipv4Module = "^.ipv4";
            @display("p=600,150");
        }
}
