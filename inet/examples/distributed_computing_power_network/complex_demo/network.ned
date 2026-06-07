package inet.examples.distributed_computing_power_network.complex_demo;

import inet.common.misc.ThruputMeteringChannel;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;

import inet.distributed_computing_power_network.node.UserGateway;
import inet.distributed_computing_power_network.node.ComputeGateway;
import inet.distributed_computing_power_network.node.UserNode;
import inet.distributed_computing_power_network.node.ComputeNode;
import inet.distributed_computing_power_network.node.ComputeRouter;

network Network
{
    @display("bgb=1925.28,1101.688");
    types:
        channel accessChannel extends ThruputMeteringChannel
        {
            delay = 0.005s;
            datarate = 100Mbps;
        }

        channel coreChannel extends ThruputMeteringChannel
        {
            delay = 0.01s;
            datarate = 100Mbps;
        }

    submodules:
        user1: UserNode {
            @display("p=184.888,160.44");
            gates:
                pppg[1];
        }
        user2: UserNode {
            @display("p=106.96001,302.544");
            gates:
                pppg[1];
        }
        user3: UserNode {
            @display("p=155.856,432.424");
            gates:
                pppg[1];
        }
        user4: UserNode {
            @display("p=1758.728,174.192");
            gates:
                pppg[1];
        }
        user5: UserNode {
            @display("p=1851.936,308.656");
            gates:
                pppg[1];
        }
        user6: UserNode {
            @display("p=1770.952,455.34402");
            gates:
                pppg[1];
        }

        userGateway1: UserGateway {
            parameters:
                multicastForwarding = true;
                @display("p=374.36002,302.544");
            gates:
                pppg[5];
        }

        userGateway2: UserGateway {
            parameters:
                multicastForwarding = true;
                @display("p=1608.984,302.544");
            gates:
                pppg[5];
        }

        router1: ComputeRouter {
            parameters:
                multicastForwarding = true;
                @display("p=734.968,302.544");
            gates:
                pppg[6];
        }

        router2: ComputeRouter {
            parameters:
                multicastForwarding = true;
                @display("p=1170.448,302.544");
            gates:
                pppg[6];
        }

        router3: ComputeRouter {
            parameters:
                multicastForwarding = true;
                @display("p=1170.448,574.528");
            gates:
                pppg[6];
        }

        router4: ComputeRouter {
            parameters:
                multicastForwarding = true;
                @display("p=734.968,562.304");
            gates:
                pppg[6];
        }

        computeGateway1: ComputeGateway {
            parameters:
                multicastForwarding = true;
                @display("p=970.28,197.11201");
            gates:
                pppg[5];
        }

        computeGateway2: ComputeGateway {
            parameters:
                multicastForwarding = true;
                @display("p=1608.984,695.24005");
            gates:
                pppg[5];
        }

        computeGateway3: ComputeGateway {
            parameters:
                multicastForwarding = true;
                @display("p=970.28,768.58405");
            gates:
                pppg[5];
        }

        computeGateway4: ComputeGateway {
            parameters:
                multicastForwarding = true;
                @display("p=374.36002,647.872");
            gates:
                pppg[5];
        }

        computeNode1: ComputeNode {
            @display("p=809.84,114.600006");
            gates:
                pppg[1];
        }
        computeNode2: ComputeNode {
            @display("p=950.416,41.256");
            gates:
                pppg[1];
        }
        computeNode3: ComputeNode {
            @display("p=1170.448,100.84801");
            gates:
                pppg[1];
        }
        computeNode4: ComputeNode {
            @display("p=1498.968,822.064");
            gates:
                pppg[1];
        }
        computeNode5: ComputeNode {
            @display("p=1628.848,904.57605");
            gates:
                pppg[1];
        }
        computeNode6: ComputeNode {
            @display("p=1761.784,822.064");
            gates:
                pppg[1];
        }
        computeNode7: ComputeNode {
            @display("p=841.92804,889.296");
            gates:
                pppg[1];
        }
        computeNode8: ComputeNode {
            @display("p=970.28,1000.84");
            gates:
                pppg[1];
        }
        computeNode9: ComputeNode {
            @display("p=1126.136,941.24805");
            gates:
                pppg[1];
        }
        computeNode10: ComputeNode {
            @display("p=195.584,789.976");
            gates:
                pppg[1];
        }
        computeNode11: ComputeNode {
            @display("p=305.6,892.35205");
            gates:
                pppg[1];
        }
        computeNode12: ComputeNode {
            @display("p=430.89603,789.976");
            gates:
                pppg[1];
        }

        configurator: Ipv4NetworkConfigurator {
            @display("p=70,30;is=s");
        }

    connections:
        user1.pppg[0] <--> accessChannel <--> userGateway1.pppg[0];
        user2.pppg[0] <--> accessChannel <--> userGateway1.pppg[1];
        user3.pppg[0] <--> accessChannel <--> userGateway1.pppg[2];
        user4.pppg[0] <--> accessChannel <--> userGateway2.pppg[0];
        user5.pppg[0] <--> accessChannel <--> userGateway2.pppg[1];
        user6.pppg[0] <--> accessChannel <--> userGateway2.pppg[2];

        userGateway1.pppg[3] <--> coreChannel <--> router1.pppg[0];
        userGateway1.pppg[4] <--> coreChannel <--> router4.pppg[0];
        userGateway2.pppg[3] <--> coreChannel <--> router2.pppg[0];
        userGateway2.pppg[4] <--> coreChannel <--> router3.pppg[0];

        router1.pppg[1] <--> coreChannel <--> router2.pppg[1];
        router2.pppg[2] <--> coreChannel <--> router3.pppg[1];
        router3.pppg[2] <--> coreChannel <--> router4.pppg[1];
        router4.pppg[2] <--> coreChannel <--> router1.pppg[2];
        router1.pppg[3] <--> coreChannel <--> router3.pppg[3];
        router2.pppg[3] <--> coreChannel <--> router4.pppg[3];

        router1.pppg[4] <--> coreChannel <--> computeGateway1.pppg[0];
        router2.pppg[4] <--> coreChannel <--> computeGateway1.pppg[1];
        router2.pppg[5] <--> coreChannel <--> computeGateway2.pppg[0];
        router3.pppg[4] <--> coreChannel <--> computeGateway2.pppg[1];
        router3.pppg[5] <--> coreChannel <--> computeGateway3.pppg[0];
        router4.pppg[4] <--> coreChannel <--> computeGateway3.pppg[1];
        router4.pppg[5] <--> coreChannel <--> computeGateway4.pppg[0];
        router1.pppg[5] <--> coreChannel <--> computeGateway4.pppg[1];

        computeGateway1.pppg[2] <--> accessChannel <--> computeNode1.pppg[0];
        computeGateway1.pppg[3] <--> accessChannel <--> computeNode2.pppg[0];
        computeGateway1.pppg[4] <--> accessChannel <--> computeNode3.pppg[0];
        computeGateway2.pppg[2] <--> accessChannel <--> computeNode4.pppg[0];
        computeGateway2.pppg[3] <--> accessChannel <--> computeNode5.pppg[0];
        computeGateway2.pppg[4] <--> accessChannel <--> computeNode6.pppg[0];
        computeGateway3.pppg[2] <--> accessChannel <--> computeNode7.pppg[0];
        computeGateway3.pppg[3] <--> accessChannel <--> computeNode8.pppg[0];
        computeGateway3.pppg[4] <--> accessChannel <--> computeNode9.pppg[0];
        computeGateway4.pppg[2] <--> accessChannel <--> computeNode10.pppg[0];
        computeGateway4.pppg[3] <--> accessChannel <--> computeNode11.pppg[0];
        computeGateway4.pppg[4] <--> accessChannel <--> computeNode12.pppg[0];
}
