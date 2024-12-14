// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_STREAM_INPUT_HPP
#define UHOBGOBLIN_UTIL_STREAM_INPUT_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <Hobgoblin/Utility/Stream_bool.hpp>
#include <Hobgoblin/Utility/Stream_extract_error.hpp>
#include <Hobgoblin/Utility/Stream_operators.hpp>

#include <cstdint>
#include <type_traits>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

/**
 * Abstract base class for a binary input stream.
 * 
 * Child classes must override:
 * - `seek`                 | Set the read position to absolute value.
 * - `seekRelative`         | Move the read position relative to its current value.
 * - `read`                 | Copy data out to a preallocated buffer and advance the read position.
 * - `extractBytes`         | Return ptr to the internal buffer, if possible, and advance the read pos.
 * - `extractBytesNoThrow`  | Same as above, but nonthrowing in case of failure.
 * - `hasError`             | Check for error state of stream.
 * - `_setError`            | (private) Set error state of stream.
 * And optionally:
 * - `isPredetermined`      | Check whether amount of data can be determined ahead of time of reading.
 * - `getDataSize`          | (if is predetermined) Get total size of data.
 * - `getRemainingDataSize` | (if is predetermined) Get size of remaining data.
 * - `getData`              | (if is predetermined) Access the internal data buffer.
 * - `getReadPosition`      | (if is predetermined) Access the internal read position.
 */
class InputStream {
public:
    //! Virtual destructor.
    virtual ~InputStream() = default;

    // Error codes
    static constexpr std::int64_t E_FAILURE = -1;
    static constexpr std::int64_t E_UNKNOWN = -2;

    ///////////////////////////////////////////////////////////////////////////
    // MARK: EXTRACTING                                                      //
    ///////////////////////////////////////////////////////////////////////////

    //! Change the current reading position of the stream.
    //!
    //! \param aPosition absolute byte offset to jump to (it is relative to the beginning
    //!                  of the data, not to the current position).
    //!
    //! \returns new position, or -1 (E_FAILURE) on failure.
    virtual std::int64_t seek(std::int64_t aPosition) = 0;

    //! Change the current reading position of the stream.
    //!
    //! \param aPosition relative byte offset to jump to (it is relative to the current position,
    //!                  not the beginning of data).
    //!
    //! \returns new position, or -1 (E_FAILURE) on failure.
    virtual std::int64_t seekRelative(std::int64_t aOffset) = 0;

    //! \brief Extracts raw data from the stream by copying it to a preallocated buffer.
    //!        Advances the read position of the stream accordingly.
    //!        Returns the number of read bytes or -1 (E_FAILURE) on failure.
    //!
    //! \param aDestination Pointer to a preallocated buffer where the data should be copied.
    //! \param aByteCount How many bytes to extract.
    //!
    //! \returns The number of bytes read (could be 0 if there was no more data in the stream,
    //!          or -1 (E_FAILURE) if the stream was already in an error state at the time of
    //!          calling this function).
    virtual int read(NeverNull<void*> aDestination, PZInteger aByteCount) = 0;

    template <class T>
    T extract();

    template <class T>
    T extractNoThrow();

    //! \brief Extracts raw data from the stream; throws on failure.
    //!
    //! Returns a pointer to a buffer of size N containing the next N bytes to be
    //! extracted from the Packet, where N is equal to `aByteCount`. These bytes
    //! can then be modified, copied, read directly or otherwise used. Clearing
    //! the Packet or appending more data to it will invalidate the returned pointer.
    //! In either case, this function will advance the Packet's internal read counter
    //! by `aByteCount`.
    //!
    //! \param aByteCount How many bytes to extract.
    //!
    //! \returns Pointer to data to be extracted, or `nullptr` if `aByteCount` is 0.
    //!
    //! \throws PacketExtractError if Packet has less than `aByteCount` bytes left
    //!         to extract. If this exception is thrown, the packet will also be
    //!         set to an invalid state.
    virtual void* extractBytes(PZInteger aByteCount) = 0;

    //! \brief Extracts raw data from the packet; does not throw on failure.
    //!
    //! Returns a pointer to a buffer of size N containing the next N bytes to be
    //! extracted from the Packet, where N is equal to `aByteCount`. These bytes
    //! can then be modified, copied, read directly or otherwise used. Clearing
    //! the Packet or appending more data to it will invalidate the returned pointer.
    //! In either case, this function will advance the Packet's internal read counter
    //! by `aByteCount`.
    //!
    //! \param aByteCount How many bytes to extract.
    //!
    //! \returns Pointer to data to be extracted, or `nullptr` if `aByteCount` is 0,
    //!          or if the Packet has less than `aByteCount` bytes left to extract
    //!          (in this case the Packet will alse be set to an invalid state).
    virtual void* extractBytesNoThrow(PZInteger aByteCount) = 0;

    //! Compile-time checker for whether a type has a compatible `operator>>`
    //! for extracting from `hg::util::InputStreamExtender`.
    template <class taType>
    class supports_extracting_of {
        template <class taType2>
        static auto test(int)
            -> decltype(std::declval<InputStreamExtender&>() >> std::declval<taType2&>(),
                        std::true_type());

        template <class>
        static auto test(...) -> std::false_type;

    public:
        static const bool value = decltype(test<taType>(0))::value;
    };

    //! Extraction operator; it has the same behaviour as `extract()`.
    //! \returns reference to self.
    template <class T>
    auto operator>>(T& aData) -> std::enable_if_t<supports_extracting_of<T>::value, InputStream&>;

    class NoThrowAdapter {
    public:
        // clang-format off
        template <class T,
                  T_ENABLE_IF(InputStream::supports_extracting_of<T&>::value)>
        NoThrowAdapter& operator>>(T& aRef) {
            aRef = _istream.extractNoThrow<T>();
            return SELF;
        }
        // clang-format on

        //! Automatic conversion to a bool-like type.
        operator StreamBool::BoolType() const {
            return (_istream) ? &StreamBool::dummy : nullptr;
        }

    private:
        friend class InputStream;

        NoThrowAdapter(InputStream& aIStream)
            : _istream{aIStream} {}

        InputStream& _istream;
    };

    //! TODO
    NoThrowAdapter noThrow() {
        return NoThrowAdapter{SELF};
    }

    ///////////////////////////////////////////////////////////////////////////
    // MARK: TESTING VALIDITY                                                //
    ///////////////////////////////////////////////////////////////////////////

    //! Check if the stream is in an error state.
    virtual bool hasError() const = 0;

    //! \brief Test the validity of the packet, for reading
    //!
    //! This operator allows to test the packet as a boolean
    //! variable, to check if a reading operation was successful.
    //!
    //! A packet will be in an invalid state if it has no more
    //! data to read.
    //!
    //! This behavior is the same as standard C++ streams.
    //!
    //! Usage example:
    //! \code
    //! float x;
    //! packet >> x;
    //! if (packet)
    //! {
    //!    // ok, x was extracted successfully
    //! }
    //!
    //! // -- or --
    //!
    //! float x;
    //! if (packet >> x)
    //! {
    //!    // ok, x was extracted successfully
    //! }
    //! \endcode
    //!
    //! Don't focus on the return type, it's equivalent to bool but
    //! it disallows unwanted implicit conversions to integer or
    //! pointer types.
    //!
    //! \return True if last data extraction from packet was successful
    //!
    //! \see endOfPacket
    //!
    operator StreamBool::BoolType() const {
        return hasError() ? nullptr : &StreamBool::dummy;
    }

    ///////////////////////////////////////////////////////////////////////////
    // MARK: INTROSPECTION                                                   //
    ///////////////////////////////////////////////////////////////////////////

    //! Check if the data in the stream is predetermined, such as when the stream has an
    //! internal data buffer. An opposite example would be a network socket, where we don't
    //! always know how much more data to expect.
    //! If this method returns false (which is also the default if not overriden), then it is
    //! expected that the following methods will return their default values as well:
    //! - `getDataSize`          (default: `E_UNKNOWN`)
    //! - `getRemainingDataSize` (default: `E_UNKNOWN`)
    //! - `getData`              (default: `nullptr`)
    //! - `getReadPosition`      (default: `E_UNKNOWN`)
    //! - `tell`                 (default: `E_UNKNOWN`)
    //! Overall there is not much utility in these methods if the stream is not predetermined
    //! and they probably shouldn't be used.
    virtual bool isPredetermined() const {
        return false;
    }

    virtual std::int64_t getDataSize() const {
        return E_UNKNOWN;
    }

    virtual std::int64_t getRemainingDataSize() const {
        return E_UNKNOWN;
    }

    //! \brief Get a pointer to the data contained in the Packet.
    //!
    //! \warning the returned pointer may become invalid after
    //! you append data to the packet, therefore it should never
    //! be stored.
    //!
    //! \returns Pointer to the data, or `nullptr` if the Packet is empty.
    //!
    //! \see getDataSize, getMutableData
    virtual const void* getData() const {
        return nullptr;
    }

    bool endOfStream() const {
        return (getRemainingDataSize() == 0);
    }

    virtual std::int64_t getReadPosition() const {
        return E_UNKNOWN;
    }

    //! Same as `getReadPosition`.
    std::int64_t tell() const {
        return getReadPosition();
    }

private:
    virtual void _setError() = 0;

    // Befriend operators
    // clang-format off
    friend InputStream& operator>>(InputStreamExtender& aStreamExtender, bool&          aData);
    friend InputStream& operator>>(InputStreamExtender& aStreamExtender, std::int8_t&   aData);
    friend InputStream& operator>>(InputStreamExtender& aStreamExtender, std::uint8_t&  aData);
    friend InputStream& operator>>(InputStreamExtender& aStreamExtender, std::int16_t&  aData);
    friend InputStream& operator>>(InputStreamExtender& aStreamExtender, std::uint16_t& aData);
    friend InputStream& operator>>(InputStreamExtender& aStreamExtender, std::int32_t&  aData);
    friend InputStream& operator>>(InputStreamExtender& aStreamExtender, std::uint32_t& aData);
    friend InputStream& operator>>(InputStreamExtender& aStreamExtender, std::int64_t&  aData);
    friend InputStream& operator>>(InputStreamExtender& aStreamExtender, std::uint64_t& aData);
    friend InputStream& operator>>(InputStreamExtender& aStreamExtender, float&         aData);
    friend InputStream& operator>>(InputStreamExtender& aStreamExtender, double&        aData);
    friend InputStream& operator>>(InputStreamExtender& aStreamExtender, std::string&   aData);
    friend InputStream& operator>>(InputStreamExtender& aStreamExtender, UnicodeString& aData);
    // clang-format on

    static void _logExtractionError(const char* aErrorMessage);
};

template <class T>
auto InputStream::operator>>(T& aData)
    -> std::enable_if_t<supports_extracting_of<T>::value, InputStream&>
//
{
    InputStreamExtender extender{SELF};
    extender >> aData;
    if (!SELF) {
        HG_THROW_TRACED(StreamExtractError,
                        0,
                        "Failed to extract data from hg::util::Packet (# of bytes remaining: {}).",
                        getRemainingDataSize());
    }
    return SELF;
}

template <class T>
T InputStream::extract() {
    T result;
    SELF >> result;
    return result;
}

template <class T>
T InputStream::extractNoThrow() {
    try {
        T                   result;
        InputStreamExtender extender{SELF};
        extender >> result;
        return result;
    } catch (const std::exception& aEx) {
        _logExtractionError(aEx.what());
        _setError();
        return {};
    }
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_STREAM_INPUT_HPP
