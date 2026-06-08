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
       double maxTransmissionBandwidth @unit(bps) = default(100Mbps);  // 用户最大传输带宽
       string taskFile = default("");                  // 场景级任务JSON文件，任务参数必须从该文件加载

	gates:	
	    input socketIn;
	    output socketOut;
}
