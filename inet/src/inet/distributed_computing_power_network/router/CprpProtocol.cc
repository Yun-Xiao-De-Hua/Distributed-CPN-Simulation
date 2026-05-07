//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//

#include "CprpProtocol.h"
#include "inet/common/ProtocolGroup.h"

namespace inet {

namespace cprp {

const Protocol& cprp = *new Protocol("cprp", "Computing-Power Routing Protocol", Protocol::NetworkLayer);

void registerCprpProtocol()
{
    static bool registered = false;
    if (!registered) {
        // INET 已占用 249-255 作为内部协议号；这里使用 248 作为本仿真的本地 CPRP 编号。
        ProtocolGroup::getIpProtocolGroup()->addProtocol(248, &cprp);
        registered = true;
    }
}

} // namespace cprp

} // namespace inet
