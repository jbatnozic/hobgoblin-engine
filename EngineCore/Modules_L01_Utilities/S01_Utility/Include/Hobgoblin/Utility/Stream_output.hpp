// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_STREAM_OUTPUT_HPP
#define UHOBGOBLIN_UTIL_STREAM_OUTPUT_HPP

#include <Hobgoblin/Common.hpp>

#include <Hobgoblin/Utility/Stream_bool.hpp>
#include <Hobgoblin/Utility/Stream_operators.hpp>

#include <type_traits>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

/**
 * Child classes must override:
 * - `appendBytes`
 * - `hasError`
 */
class OutputStream {
public:
    virtual ~OutputStream() = default;

    ///////////////////////////////////////////////////////////////////////////
    // MARK: APPENDING                                                       //
    ///////////////////////////////////////////////////////////////////////////

    //! TODO (description)
    template <class T>
    OutputStream& append(T&& aData);

    //! \brief Append data to the end of the packet
    //!
    //! \param aData      Pointer to the sequence of bytes to append
    //! \param aByteCount Number of bytes to append
    //!
    //! \see clear
    //! \see getReadPosition
    virtual OutputStream& appendBytes(const void* aData, PZInteger aByteCount) = 0;

    //! Compile-time checker for whether a type has a compatible `operator<<`
    //! for appending into `hg::util::OutputStreamExtender`.
    template <class taType>
    class supports_appending_of {
        template <class taType2>
        static auto test(int)
            -> decltype(std::declval<OutputStreamExtender&>() << std::declval<const taType2&>(),
                        std::true_type());

        template <class>
        static auto test(...) -> std::false_type;

    public:
        static const bool value = decltype(test<taType>(0))::value;
    };

    template <class T>
    auto operator<<(T&& aData) -> std::enable_if_t<supports_appending_of<T>::value, OutputStream&>;

    class NoThrowAdapter {
    public:        
        // clang-format off
        template <class T,
                  T_ENABLE_IF(OutputStream::supports_appending_of<T&>::value)>
        NoThrowAdapter& operator<<(T& aRef) {
            _ostream.append(aRef);
            return SELF;
        }
        // clang-format on

        //! TODO
        operator StreamBool::BoolType() const {
            return (_ostream) ? &StreamBool::dummy : nullptr;
        }

    private:
        friend class OutputStream;

        NoThrowAdapter(OutputStream& aOStream)
            : _ostream{aOStream} {}

        void _dummy() {}

        OutputStream& _ostream;
    };

    NoThrowAdapter noThrow() {
        return NoThrowAdapter{SELF};
    }

    ///////////////////////////////////////////////////////////////////////////
    // MARK: TESTING VALIDITY                                                //
    ///////////////////////////////////////////////////////////////////////////

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

private:
    // Befriend operators
    //clang-format off
    friend OutputStream& operator<<(OutputStreamExtender& aStreamExtender, bool                 aData);
    friend OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::int8_t          aData);
    friend OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::uint8_t         aData);
    friend OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::int16_t         aData);
    friend OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::uint16_t        aData);
    friend OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::int32_t         aData);
    friend OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::uint32_t        aData);
    friend OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::int64_t         aData);
    friend OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::uint64_t        aData);
    friend OutputStream& operator<<(OutputStreamExtender& aStreamExtender, float                aData);
    friend OutputStream& operator<<(OutputStreamExtender& aStreamExtender, double               aData);
    friend OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::string_view     aData);
    friend OutputStream& operator<<(OutputStreamExtender& aStreamExtender, const UnicodeString& aData);
    // OutputStream& operator<<(OutputStreamExtender& aPacket, const Packet&        aData);
    //clang-format on

    void _dummy() {}
};

template <class T>
OutputStream& OutputStream::append(T&& aData) {
    return (SELF << std::forward<T>(aData));
}

template <class T>
auto OutputStream::operator<<(T&& aData)
    -> std::enable_if_t<supports_appending_of<T>::value, OutputStream&>
//
{
    OutputStreamExtender extender{SELF};
    extender << std::forward<T>(aData);
    return SELF;
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_STREAM_OUTPUT_HPP
