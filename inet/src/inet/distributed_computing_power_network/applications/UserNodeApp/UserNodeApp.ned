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
       int taskId = default(1);                         // 任务编号
       int taskComputingType = default(0);              // 请求的算力资源类型，0表示CPU，1表示GPU
       double taskRequiredStorage = default(100);       // 任务所需存储空间(MB)
       double taskDataSize = default(1000);             // 任务数据量(MB)
       double taskComputingAmount = default(1000);      // 任务计算量(FLOPs)
       double taskDelayTolerance @unit(s) = default(1s); // 时延忍耐值(sec)
       double taskBudget = default(100);                // 任务预算

	gates:	
	    input socketIn;
	    output socketOut;
}
