
#include <Hobgoblin/RigelNet/Packet_wrapper.hpp>
#include <SFML/Network.hpp>

#include <cstring>
#include <cstdint>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

namespace {

struct SfmlPacketClone {
    std::vector<char> data;    ///< Data stored in the packet
    std::size_t       readPos; ///< Current reading position in the packet
    std::size_t       sendPos; ///< Current send position in the packet (for handling partial sends)
    bool              isValid;

    // Must also copy virtual methods so the vtable is generated
    virtual const void* onSend(std::size_t& size) { 
        size = 0;
        return nullptr; 
    }

    virtual void onReceive(const void* data, std::size_t size) {
    }
};

std::vector<char>& SfmlPacketAccess_GetDataVector(sf::Packet& packet) {
    return reinterpret_cast<SfmlPacketClone&>(packet).data;
}

const std::vector<char>& SfmlPacketAccess_GetDataVector(const sf::Packet& packet) {
    return reinterpret_cast<const SfmlPacketClone&>(packet).data;
}

std::size_t& SfmlPacketAccess_GetReadPosition(sf::Packet& packet) {
    return reinterpret_cast<SfmlPacketClone&>(packet).readPos;
}

std::size_t SfmlPacketAccess_GetReadPosition(const sf::Packet& packet) {
    return reinterpret_cast<const SfmlPacketClone&>(packet).readPos;
}

std::size_t& SfmlPacketAccess_GetSendPosition(sf::Packet& packet) {
    return reinterpret_cast<SfmlPacketClone&>(packet).sendPos;
}

std::size_t SfmlPacketAccess_GetSendPosition(const sf::Packet& packet) {
    return reinterpret_cast<const SfmlPacketClone&>(packet).sendPos;
}

bool& SfmlPacketAccess_GetIsValid(sf::Packet& packet) {
    return reinterpret_cast<SfmlPacketClone&>(packet).isValid;
}

bool SfmlPacketAccess_GetIsValid(const sf::Packet& packet) {
    return reinterpret_cast<const SfmlPacketClone&>(packet).isValid;
}

void* SfmlPacketAccess_ExtractBytes(sf::Packet& packet, std::size_t byteCount) {
    auto& data = SfmlPacketAccess_GetDataVector(packet);
    auto& readPos = SfmlPacketAccess_GetReadPosition(packet);

    void* const rv = &(data[readPos]);

    if (readPos + byteCount > data.size()) {
        SfmlPacketAccess_GetIsValid(packet) = false;
    }
    else {
        readPos += byteCount;
    }

    return rv;
}

} // namespace

// TODO WTF is up with this file?????

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

void* Packet::extractBytes(std::size_t byteCount) {
    return SfmlPacketAccess_ExtractBytes(SELF, byteCount);
}

PacketBase& operator<<(PacketBase& dstPacket, const Packet& srcPacket) {
    dstPacket << static_cast<std::uint32_t>(srcPacket.getDataSize());
    dstPacket.append(srcPacket.getData(), srcPacket.getDataSize());

    return dstPacket;
}

PacketBase& operator>>(PacketBase& srcPacket, Packet& dstPacket) {
    std::uint32_t dataSizeInBytes;
    srcPacket >> dataSizeInBytes;

    void* const data = SfmlPacketAccess_ExtractBytes(srcPacket, dataSizeInBytes);

    if (srcPacket) {
        dstPacket.append(data, dataSizeInBytes);
    }

    return srcPacket;
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
