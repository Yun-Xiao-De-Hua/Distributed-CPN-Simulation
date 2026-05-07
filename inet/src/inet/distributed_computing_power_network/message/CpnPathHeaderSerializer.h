//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//

#ifndef INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_MESSAGE_CPNPATHHEADERSERIALIZER_H_
#define INET_DISTRIBUTED_COMPUTING_POWER_NETWORK_MESSAGE_CPNPATHHEADERSERIALIZER_H_

#include "inet/common/packet/serializer/ChunkSerializer.h"

namespace inet {

class INET_API CpnPathHeaderSerializer : public ChunkSerializer
{
  public:
    virtual void serialize(MemoryOutputStream& stream, const Ptr<const Chunk>& chunk, b offset, b length) const override;
    virtual const Ptr<Chunk> deserialize(MemoryInputStream& stream, const std::type_info& typeInfo) const override;
};

} // namespace inet

#endif
