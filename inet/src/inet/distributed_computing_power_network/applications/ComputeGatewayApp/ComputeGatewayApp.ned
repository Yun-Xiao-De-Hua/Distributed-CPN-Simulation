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
        input cprpControlIn;
        output socketOut;
}
