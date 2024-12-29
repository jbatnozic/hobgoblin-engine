// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_STREAM_BUFFER_HPP
#define UHOBGOBLIN_UTIL_STREAM_BUFFER_HPP

#include <Hobgoblin/Utility/Stream_input.hpp>
#include <Hobgoblin/Utility/Stream_output.hpp>
#include <Hobgoblin/Utility/Stream_nothrow.hpp>

#include <Hobgoblin/Math/Core.hpp>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

//! Implements the `OutputStream` and `InputStream` interfaces by using a simple internal
//! data buffer (std::vector<char> or equivalent).
//!
//! \note this class is based on SFML's Packet class and the principles of using it are very similar
//!       (see https://www.sfml-dev.org/documentation/3.0.0/classsf_1_1Packet.html).
class BufferStream
    : public OutputStream
    , public InputStream {
public:
    //! \brief Default constructor
    //!
    //! Creates an empty BufferStream.
    BufferStream() = default;

    //! \brief Copy constructor.
    BufferStream(const BufferStream& aOther) = default;

    //! \brief Move constructor.
    BufferStream(BufferStream&& aOther) = default;

    //! \brief Copy assignment operator.
    BufferStream& operator=(const BufferStream& aOther) = default;

    //! \brief Move assignment operator.
    BufferStream& operator=(BufferStream&& aOther) = default;

    //! \brief Destructor.
    ~BufferStream() override = default;

    //! \brief Clear the stream (reset its whole state).
    //!
    //! After calling clear(), the stream is empty.
    //!
    //! \see append
    void clear() {
        _buffer.clear();
        _readPos        = 0;
        _readErrorLevel = 0;
    }

    //! \brief Get a non-const pointer to the data contained in the BufferStream.
    //!
    //! \warning the returned pointer may become invalid after you append data to 
    //!          the buffer, therefore it should never be stored.
    //!
    //! \warning the returned pointer will allow you to edit the data stored in the
    //!          buffer. DO NOT do this unless you're sure you know what you're doing.
    //!
    //! \returns Pointer to the data, or `nullptr` if the BufferStream is empty.
    //!
    //! \see getData, getDataSize
    HG_NODISCARD void* getMutableData();

    ///////////////////////////////////////////////////////////////////////////
    // MARK: INTROSPECTION                                                   //
    ///////////////////////////////////////////////////////////////////////////

    HG_NODISCARD bool isPredetermined() const override {
        return true;
    }

    HG_NODISCARD std::int64_t getDataSize() const override {
        return static_cast<std::int64_t>(_buffer.size());
    }

    HG_NODISCARD std::int64_t getRemainingDataSize() const override {
        return static_cast<std::int64_t>(_buffer.size() - _readPos);
    }

    HG_NODISCARD const void* getData() const override {
        return _buffer.empty() ? nullptr : _buffer.data();
    }

    HG_NODISCARD std::int64_t getReadPosition() const override {
        return static_cast<std::int64_t>(_readPos);
    }

    ///////////////////////////////////////////////////////////////////////////
    // MARK: STATE CHECKING                                                  //
    ///////////////////////////////////////////////////////////////////////////

    HG_NODISCARD bool hasWriteError() const override {
        return (_readErrorLevel == E_BADSTATE);
    }

    HG_NODISCARD bool hasReadError() const override {
        return (_readErrorLevel < 0);
    }

    HG_NODISCARD bool clearWriteError() override {
        if (_readErrorLevel == E_BADSTATE) {
            return false;
        }
        return true;
    }

    HG_NODISCARD bool clearReadError() override {
        if (_readErrorLevel == E_BADSTATE) {
            return false;
        }
        _readErrorLevel = 0;
        return true;
    }

    HG_NODISCARD bool isGood() const override {
        return (_readErrorLevel != E_BADSTATE);
    }

    ///////////////////////////////////////////////////////////////////////////
    // MARK: NOTHROW ADAPTER                                                 //
    ///////////////////////////////////////////////////////////////////////////

    HG_DISAMBIGUATE_NOTHROW(BufferStream)

private:
    std::vector<std::uint8_t> _buffer;             //!< Buffer of stored data
    std::size_t               _readPos        = 0; //!< Current reading position in the stream
    std::int64_t              _readErrorLevel = 0; //!< Reading state of the stream

    // ===== I/O Operations =====

    std::int64_t _write(NeverNull<const void*> aData,
                        std::int64_t           aByteCount,
                        bool                   aAllowPartal) override;

    std::int64_t _seek(std::int64_t aPosition) override;

    std::int64_t _seekRelative(std::int64_t aOffset) override;

    std::int64_t _read(NeverNull<void*> aDestination,
                       std::int64_t     aByteCount,
                       bool             aAllowPartal) override;

    const void* _readInPlace(std::int64_t aByteCount) override;

    std::int64_t _readInPlaceNoThrow(std::int64_t aByteCount, const void** aResult) override;

    // ===== State Checking =====

    void _setReadError() override {
        _readErrorLevel = std::min(_readErrorLevel, E_FAILURE);
    }

    void _setWriteError() override {}

    void _setNotGood() override {
        _readErrorLevel = E_BADSTATE;
    }
};

OutputStream& operator<<(OutputStreamExtender& aBufferStreamExt, const BufferStream& aData);

InputStream& operator>>(InputStreamExtender& aBufferStreamExt, BufferStream& aData);

//! Helper for PackArgs (see below).
// clang-format off
template <class ...taNoArgs,
          T_ENABLE_IF(sizeof...(taNoArgs) == 0)>
// clang-format on
void PackArgs(BufferStream&) {}

//! Pack all arguments into the given BufferStream, in order of appearance (left to right).
template <class taArgsHead, class... taArgsRest>
void PackArgs(BufferStream& aBufferStream, taArgsHead&& aArgsHead, taArgsRest&&... aArgsRest) {
    aBufferStream.append(std::forward<taArgsHead>(aArgsHead));
    PackArgs<taArgsRest...>(aBufferStream, std::forward<taArgsRest>(aArgsRest)...);
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_STREAM_BUFFER_HPP
