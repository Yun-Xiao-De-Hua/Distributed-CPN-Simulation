package inet.examples.distributed_computing_power_network.simple_demo;

import inet.common.misc.ThruputMeteringChannel;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.ospfv2.OspfRouter;
import inet.node.inet.StandardHost;

import inet.distributed_computing_power_network.node.UserGateway;
import inet.distributed_computing_power_network.node.ComputeGateway;
import inet.distributed_computing_power_network.node.UserNode;
import inet.distributed_computing_power_network.node.ComputeNode;
import inet.distributed_computing_power_network.node.ComputeRouter;

network Network
{
    @display("bgb=800,500"); // 扩大画布以容纳更多节点
    types:
        channel testChannel extends ThruputMeteringChannel
        {
            delay = 0.01s;
            datarate = 10Mbps;
        }

    submodules:
        user1: UserNode {
            parameters:
                @display("p=77,228");
            gates:
                pppg[1];
        }

        userGateway1: UserGateway {
            parameters:
                multicastForwarding = true;
                @display("p=212,228");
            gates:
                pppg[2];
        }

        router1: ComputeRouter {
            parameters:
                multicastForwarding = true;
                @display("p=350,228");
            gates:
                pppg[3];
        }

        // 算力网关1
        computeGateway1: ComputeGateway {
            parameters:
                multicastForwarding = true;                
                @display("p=500,150");
            gates:
                pppg[3]; 
        }

        // 算力网关2
        computeGateway2: ComputeGateway {
            parameters:
                multicastForwarding = true;                
                @display("p=500,350");
            gates:
                pppg[3];
        }

        // 算力网关1下的两个节点
        computeNode1: ComputeNode {
            parameters:
                multicastForwarding = true;                
                @display("p=650,100");
            gates:
                pppg[1];
        }
        computeNode2: ComputeNode {
            parameters:
                multicastForwarding = true;               
                @display("p=650,200");
            gates:
                pppg[1];
        }

        // 算力网关2下的两个节点
        computeNode3: ComputeNode {
            parameters:
                multicastForwarding = true;                
                @display("p=650,300");
            gates:
                pppg[1];
        }
        computeNode4: ComputeNode {
            parameters:
                multicastForwarding = true;                
                @display("p=650,400");
            gates:
                pppg[1];
        }

        configurator: Ipv4NetworkConfigurator {
            @display("p=77,31;is=s");
        }

    connections:
        user1.pppg[0] <--> testChannel <--> userGateway1.pppg[0];
        userGateway1.pppg[1] <--> testChannel <--> router1.pppg[0];
        
        // 路由到网关
        router1.pppg[1] <--> testChannel <--> computeGateway1.pppg[1];
        router1.pppg[2] <--> testChannel <--> computeGateway2.pppg[1];
        
        // 网关1连接两个算力节点
        computeGateway1.pppg[0] <--> testChannel <--> computeNode1.pppg[0];
        computeGateway1.pppg[2] <--> testChannel <--> computeNode2.pppg[0];
        
        // 网关2连接两个算力节点
        computeGateway2.pppg[0] <--> testChannel <--> computeNode3.pppg[0];
        computeGateway2.pppg[2] <--> testChannel <--> computeNode4.pppg[0];
}