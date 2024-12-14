// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_PACKET_HPP
#define UHOBGOBLIN_UTIL_PACKET_HPP

#include <Hobgoblin/Utility/Streams.hpp>

#include <Hobgoblin/Math/Core.hpp>

#include <cstring>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

using PacketExtractError = StreamExtractError;

class Packet final
    : public OutputStream
    , public InputStream {
public:
    //! \brief Default constructor
    //!
    //! Creates an empty packet.
    Packet() = default;

    //! \brief Copy constructor.
    Packet(const Packet& aOther) = default;

    //! \brief Move constructor.
    Packet(Packet&& aOther) = default;

    //! \brief Copy assignment operator.
    Packet& operator=(const Packet& aOther) = default;

    //! \brief Move assignment operator.
    Packet& operator=(Packet&& aOther) = default;

    //! \brief Destructor.
    ~Packet() override = default;

    //! \brief Clear the packet (reset its whole state).
    //!
    //! After calling clear(), the packet is empty.
    //!
    //! \see append
    void clear() {
        _buffer.clear();
        _readPos = 0;
        _isValid = true;
    }

    //! Same as `endOfStream`.
    bool endOfPacket() const {
        return endOfStream();
    }

    //! \brief Get a non-const pointer to the data contained in the Packet.
    //! 
    //! \warning the returned pointer may become invalid after
    //! you append data to the packet, therefore it should never
    //! be stored.
    //! 
    //! \warning the returned pointer will allow you to edit the data stored
    //! in the packet. DO NOT do this unless you're sure you know what you're
    //! doing.
    //! 
    //! \returns Pointer to the data, or `nullptr` if the Packet is empty.
    //!
    //! \see getData, getDataSize
    void* getMutableData();

    ///////////////////////////////////////////////////////////////////////////
    // MARK: OUTPUT STREAM                                                   //
    ///////////////////////////////////////////////////////////////////////////

    Packet& appendBytes(NeverNull<const void*> aData, PZInteger aByteCount) override;

    ///////////////////////////////////////////////////////////////////////////
    // MARK: INPUT STREAM                                                    //
    ///////////////////////////////////////////////////////////////////////////

    std::int64_t seek(std::int64_t aPosition) override;

    std::int64_t seekRelative(std::int64_t aOffset) override;

    int read(NeverNull<void*> aDestination, PZInteger aByteCount) override;

    void* extractBytes(PZInteger aByteCount) override;

    void* extractBytesNoThrow(PZInteger aByteCount) override;

    bool isPredetermined() const override {
        return true;
    }

    std::int64_t getDataSize() const override {
        return static_cast<std::int64_t>(_buffer.size());
    }

    std::int64_t getRemainingDataSize() const override {
        return static_cast<std::int64_t>(_buffer.size() - _readPos);
    }

    const void* getData() const override {
        return _buffer.empty() ? nullptr : _buffer.data();
    }

    std::int64_t getReadPosition() const override {
        return static_cast<std::int64_t>(_readPos);
    }

    ///////////////////////////////////////////////////////////////////////////
    // MARK: TESTING VALIDITY                                                //
    ///////////////////////////////////////////////////////////////////////////

    bool hasError() const override {
        return !_isValid;
    }

    operator StreamBool::BoolType() const {
        return hasError() ? nullptr : &StreamBool::dummy;
    }

    ///////////////////////////////////////////////////////////////////////////
    // MARK: NOTHROW ADAPTER                                                 //
    ///////////////////////////////////////////////////////////////////////////

    HG_DISAMBIGUATE_NOTHROW(Packet)

private:
    std::vector<std::uint8_t> _buffer;         //!< Buffer of data stored in the packet
    std::size_t               _readPos = 0;    //!< Current reading position in the packet
    bool                      _isValid = true; //!< Reading state of the packet

    void _setError() override {
        _isValid = false;
    }

    friend InputStream& operator>>(InputStreamExtender& aPacketExt, Packet& aData);
};

OutputStream& operator<<(OutputStreamExtender& aPacketExt, const Packet& aData);

InputStream& operator>>(InputStreamExtender& aPacketExt, Packet& aData);

//! Helper for PackArgs (see below).
// clang-format off
template <class ...taNoArgs,
          T_ENABLE_IF(sizeof...(taNoArgs) == 0)>
// clang-format on
void PackArgs(Packet&) {}

//! Pack all arguments into the given Packet, in order of appearance (left to right).
template <class taArgsHead, class ...taArgsRest>
void PackArgs(Packet& aPacket, taArgsHead&& aArgsHead, taArgsRest&&... aArgsRest) {
    aPacket.append(std::forward<taArgsHead>(aArgsHead));
    PackArgs<taArgsRest...>(aPacket, std::forward<taArgsRest>(aArgsRest)...);
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_PACKET_HPP
