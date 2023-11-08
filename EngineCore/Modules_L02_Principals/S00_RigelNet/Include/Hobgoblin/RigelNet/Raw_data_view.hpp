#ifndef UHOBGOBLIN_RN_RAW_DATA_VIEW_HPP
#define UHOBGOBLIN_RN_RAW_DATA_VIEW_HPP

#include <Hobgoblin/Utility/Packet.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

//! Utility class to enable sending and receiving raw byte data with RigelNet
class RN_RawDataView {
public:
    RN_RawDataView(const void* data, std::size_t dataByteCount)
        : _data{data}
        , _dataByteCount{static_cast<decltype(_dataByteCount)>(dataByteCount)}
    {
    }

    RN_RawDataView()
        : RN_RawDataView{nullptr, 0}
    {
    }

    const void* getData() const noexcept {
        return _data;
    }

    std::size_t getDataSize() const noexcept {
        return _dataByteCount;
    }

    friend util::PacketBase& operator<<(util::PacketBase& packet, const RN_RawDataView& self) {
        if (self._data != nullptr && self._dataByteCount > 0) {
            packet << self._dataByteCount;
            packet.append(self._data, self._dataByteCount);
        }
        return packet;
    }

    friend util::PacketBase& operator>>(util::PacketBase& packet, RN_RawDataView& self) {
        // TODO: Ugly hack with reinterpret cast
        util::Packet& extendedPacket = reinterpret_cast<util::Packet&>(packet);
        extendedPacket >> self._dataByteCount;
        self._data = extendedPacket.extractBytes(self._dataByteCount);

        return packet;
    }

private:
    const void* _data;
    std::uint32_t _dataByteCount;
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_RAW_DATA_VIEW_HPP