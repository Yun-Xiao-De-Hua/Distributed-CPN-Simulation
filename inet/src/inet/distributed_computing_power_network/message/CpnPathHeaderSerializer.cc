//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//

#include "CpnPathHeaderSerializer.h"

#include <cstring>

#include "inet/common/packet/serializer/ChunkSerializerRegistry.h"
#include "inet/distributed_computing_power_network/message/CpnPathHeader_m.h"

namespace inet {

Register_Serializer(CpnPathHeader, CpnPathHeaderSerializer);

static const B CPN_PATH_HEADER_BASE_LENGTH = B(64);
static const B CPN_PATH_ADDRESS_LENGTH = B(16);

static B getCpnPathHeaderSerializedLength(size_t hopCount, size_t sidCount)
{
    return B(CPN_PATH_HEADER_BASE_LENGTH.get() + CPN_PATH_ADDRESS_LENGTH.get() * (hopCount + sidCount));
}

static void writeDouble(MemoryOutputStream& stream, double value)
{
    uint64_t bits;
    std::memcpy(&bits, &value, sizeof(bits));
    stream.writeUint64Be(bits);
}

static double readDouble(MemoryInputStream& stream)
{
    uint64_t bits = stream.readUint64Be();
    double value;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

static void writePadding(MemoryOutputStream& stream, B length)
{
    if (length < B(0))
        throw cRuntimeError("CpnPathHeaderSerializer: negative padding length");
    stream.writeByteRepeatedly(0, B(length).get());
}

static void readPadding(MemoryInputStream& stream, B length)
{
    if (length < B(0))
        throw cRuntimeError("CpnPathHeaderSerializer: negative padding length");
    for (int64_t i = 0; i < B(length).get(); i++)
        stream.readByte();
}

static void writeL3Address(MemoryOutputStream& stream, const L3Address& address)
{
    stream.writeUint32Be((uint32_t)address.getType());
    switch (address.getType()) {
        case L3Address::NONE:
            writePadding(stream, B(12));
            break;
        case L3Address::IPv4:
            stream.writeIpv4Address(address.toIpv4());
            writePadding(stream, B(8));
            break;
        default:
            throw cRuntimeError("CpnPathHeaderSerializer currently supports only IPv4 L3Address values, got type %d", (int)address.getType());
    }
}

static L3Address readL3Address(MemoryInputStream& stream)
{
    auto type = (L3Address::AddressType)stream.readUint32Be();
    switch (type) {
        case L3Address::NONE:
            readPadding(stream, B(12));
            return L3Address();
        case L3Address::IPv4: {
            auto address = L3Address(stream.readIpv4Address());
            readPadding(stream, B(8));
            return address;
        }
        default:
            throw cRuntimeError("CpnPathHeaderSerializer currently supports only IPv4 L3Address values, got type %d", (int)type);
    }
}

void CpnPathHeaderSerializer::serialize(MemoryOutputStream& stream, const Ptr<const Chunk>& chunk, b offset, b length) const
{
    const auto& pathHeader = staticPtrCast<const CpnPathHeader>(chunk);
    if (offset.get() % 8 != 0 || (length != b(-1) && length.get() % 8 != 0))
        throw cRuntimeError("CpnPathHeaderSerializer: bit-level partial serialization is not supported");

    B chunkLength = B(pathHeader->getChunkLength());
    if (chunkLength < CPN_PATH_HEADER_BASE_LENGTH)
        throw cRuntimeError("CpnPathHeaderSerializer: invalid header length %lld B", (long long)chunkLength.get());

    B minimumLength = getCpnPathHeaderSerializedLength(pathHeader->getHopAddressArraySize(), pathHeader->getSidListArraySize());
    if (chunkLength < minimumLength)
        throw cRuntimeError("CpnPathHeaderSerializer: header length %lld B is shorter than required %lld B", (long long)chunkLength.get(), (long long)minimumLength.get());

    MemoryOutputStream headerStream(chunkLength);
    headerStream.writeUint32Be((uint32_t)pathHeader->getMode());
    headerStream.writeUint32Be((uint32_t)pathHeader->getUserId());
    headerStream.writeUint32Be((uint32_t)pathHeader->getTaskId());

    headerStream.writeUint32Be((uint32_t)pathHeader->getHopAddressArraySize());
    for (size_t i = 0; i < pathHeader->getHopAddressArraySize(); i++)
        writeL3Address(headerStream, pathHeader->getHopAddress(i));

    writeL3Address(headerStream, pathHeader->getUserGatewayAddress());
    writeDouble(headerStream, pathHeader->getRequiredBandwidth());

    headerStream.writeUint32Be((uint32_t)pathHeader->getSidListArraySize());
    for (size_t i = 0; i < pathHeader->getSidListArraySize(); i++)
        writeL3Address(headerStream, pathHeader->getSidList(i));

    headerStream.writeUint32Be((uint32_t)pathHeader->getCurrentHopIndex());
    writePadding(headerStream, chunkLength - B(headerStream.getLength()));

    std::vector<uint8_t> bytes;
    headerStream.copyData(bytes);

    B outputOffset = B(offset);
    B outputLength = length == b(-1) ? chunkLength - outputOffset : B(length);
    stream.writeBytes(bytes, outputOffset, outputLength);
}

const Ptr<Chunk> CpnPathHeaderSerializer::deserialize(MemoryInputStream& stream, const std::type_info& typeInfo) const
{
    (void)typeInfo;
    auto startPosition = stream.getPosition();
    B remainingLength = B(stream.getRemainingLength());
    if (remainingLength < CPN_PATH_HEADER_BASE_LENGTH)
        throw cRuntimeError("CpnPathHeaderSerializer: stream is too short for CpnPathHeader: %lld B", (long long)remainingLength.get());

    auto pathHeader = makeShared<CpnPathHeader>();
    pathHeader->setMode((int32_t)stream.readUint32Be());
    pathHeader->setUserId((int32_t)stream.readUint32Be());
    pathHeader->setTaskId((int32_t)stream.readUint32Be());

    auto hopCount = stream.readUint32Be();
    pathHeader->setHopAddressArraySize(hopCount);
    for (uint32_t i = 0; i < hopCount; i++)
        pathHeader->setHopAddress(i, readL3Address(stream));

    pathHeader->setUserGatewayAddress(readL3Address(stream));
    pathHeader->setRequiredBandwidth(readDouble(stream));

    auto sidCount = stream.readUint32Be();
    B minimumLength = getCpnPathHeaderSerializedLength(hopCount, sidCount);
    if (remainingLength < minimumLength)
        throw cRuntimeError("CpnPathHeaderSerializer: stream length %lld B is shorter than required %lld B", (long long)remainingLength.get(), (long long)minimumLength.get());

    pathHeader->setSidListArraySize(sidCount);
    for (uint32_t i = 0; i < sidCount; i++)
        pathHeader->setSidList(i, readL3Address(stream));

    pathHeader->setCurrentHopIndex((int32_t)stream.readUint32Be());
    B consumedLength = B(stream.getPosition() - startPosition);
    readPadding(stream, minimumLength - consumedLength);
    pathHeader->setChunkLength(minimumLength);
    return pathHeader;
}

} // namespace inet
