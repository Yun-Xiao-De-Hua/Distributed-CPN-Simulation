//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 

package inet.distributed_computing_power_network.routing;

import inet.distributed_computing_power_network.routing.common.CprpProcessorBase;

simple ComputeGatewayProcessor extends CprpProcessorBase
{
    parameters:
        @class(ComputeGatewayProcessor);
        int computeNodePort = default(5000);

    gates:
        output cancelOut;
}
