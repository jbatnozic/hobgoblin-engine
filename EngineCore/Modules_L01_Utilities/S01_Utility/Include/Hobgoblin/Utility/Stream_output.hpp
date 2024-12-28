// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_STREAM_OUTPUT_HPP
#define UHOBGOBLIN_UTIL_STREAM_OUTPUT_HPP

#include <Hobgoblin/Common.hpp>

#include <Hobgoblin/Utility/Stream_base.hpp>
#include <Hobgoblin/Utility/Stream_errors.hpp>
#include <Hobgoblin/Utility/Stream_operators.hpp>

#include <cstdint>
#include <type_traits>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

/**
 * Abstract base class for a binary output stream.
 *
 * Child classes must override*:
 * - State checking:
 *   - `hasWriteError`   | Check for read error state of stream.
 *   - `clearWriteError` | Clear the read error state of stream.
 *   - `_setWriteError`  | Set read error state of stream.
 *   - `isGood`          | Check if the stream is in a valid state for I/O operations.
 *   - `_setNotGood`     | Set the stream to the permanently invalid state.
 *
 * - I/O operations:
 *   - `_write`          |
 *
 * *-Method names starting with an underscore are private implementations of public methods.
 *   They can be overriden regardless.
 */
class OutputStream : public StreamBase {
public:
    virtual ~OutputStream() = default;

    ///////////////////////////////////////////////////////////////////////////
    // MARK: I/O OPERATIONS                                                  //
    ///////////////////////////////////////////////////////////////////////////

    //! \brief write raw data to the stream.
    //!        Returns the number of bytes written or an error code (see `StreamBase`) on failure.
    //!
    //! \param aDestination Pointer to a buffer from which to copy the data.
    //! \param aByteCount How many bytes to copy.
    //! \param aAllowPartal Whether partial (smaller than aByteCount - zero included) writes are allowed.
    //!
    //! \returns The number of bytes written (could be 0 if the stream could receive no data at
    //!          all, or a negative error code (see `StreamBase`) on failure).
    HG_NODISCARD std::int64_t write(NeverNull<const void*> aData,
                                    std::int64_t           aByteCount,
                                    bool                   aAllowPartal = false);

    //! Appends an object of type `T` to the stream (assuming it has the proper `operator<<`
    //! defined.
    //! \throws StreamWriteError on failure. If this exception is thrown, the write error flag
    //!                          will also be set to an invalid state.
    template <class T>
    void append(T&& aData);

    //! Appends an object of type `T` to the stream (assuming it has the proper `operator<<`
    //! defined. This method throws no exceptions, so you will have to check `hasWriteError()`
    //! after one or several calls to this method - if it returns `false`, then all appends
    //! since the last time it returned `true` should be considered invalid.
    template <class T>
    void appendNoThrow(T&& aData);

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

    //! Appending operator; it has the same behaviour as `append()`.
    //! \returns reference to self.
    template <class T>
    auto operator<<(T&& aData) -> std::enable_if_t<supports_appending_of<T>::value, OutputStream&>;

    //! TODO
    class NoThrowAdapter {
    public:
        // clang-format off
        template <class T,
                  T_ENABLE_IF(OutputStream::supports_appending_of<T&>::value)>
        NoThrowAdapter& operator<<(T& aRef) {
            _ostream.appendNoThrow(aRef);
            return SELF;
        }
        // clang-format on

        operator StrongBool() const {
            return _ostream.hasWriteError() ? SBOOL_FALSE : SBOOL_TRUE;
        }

    private:
        friend class OutputStream;

        NoThrowAdapter(OutputStream& aOStream)
            : _ostream{aOStream} {}

        OutputStream& _ostream;
    };

    //! TODO
    NoThrowAdapter noThrow() {
        return NoThrowAdapter{SELF};
    }

    ///////////////////////////////////////////////////////////////////////////
    // MARK: STATE CHECKING                                                  //
    ///////////////////////////////////////////////////////////////////////////

    //! Check if the stream's write error flag is set.
    //!
    //! \note if `true`, that means that all non-throwing writes since the last time
    //!       that this method was called and returned `false` are invalid. Furthermore,
    //!       all future writes until the write error is successfully cleared will also
    //!       be invalid.
    //!
    //! \note if `isGood` returns `false`, then this method shall return `true`.
    HG_NODISCARD virtual bool hasWriteError() const = 0;

    //! Attempt to clear the write error in order to unblock future write.
    //! \returns `true` is successful, `false` on failure.
    HG_NODISCARD virtual bool clearWriteError() = 0;

    //! Checks if the stream as a whole is in a valid state. If this method returns `false`,
    //! then the stream has encountered an unrecoverable error state and all further operations
    //! (other than to destroy it) are invalid.
    HG_NODISCARD virtual bool isGood() const = 0;

private:
    //! Implementation for `_setWriteError`.
    virtual void _setWriteError() = 0;
    //! Implementation for `_setNotGood`.
    virtual void _setNotGood() = 0;
    //! Implementation for `write`.
    virtual std::int64_t _write(NeverNull<const void*> aData,
                                std::int64_t           aByteCount,
                                bool                   aAllowPartal) = 0;

    static void _logAppendingError(const char* aErrorMessage);
};

///////////////////////////////////////////////////////////////////////////
// MARK: INLINE FUNCTION IMPLEMENTATIONS                                 //
///////////////////////////////////////////////////////////////////////////

inline std::int64_t OutputStream::write(NeverNull<const void*> aData,
                                        std::int64_t           aByteCount,
                                        bool                   aAllowPartal) {
    const auto result = _write(aData, aByteCount, aAllowPartal);
    if (result >= 0) {
        // Do nothing
    } else if (result == E_FAILURE) {
        _setWriteError();
    } else if (result == E_BADSTATE) {
        _setNotGood();
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////
// MARK: TEMPLATE FUNCTION IMPLEMENTATIONS                               //
///////////////////////////////////////////////////////////////////////////

template <class T>
void OutputStream::append(T&& aData) {
    SELF << std::forward<T>(aData);
    if (hasWriteError()) {
        HG_THROW_TRACED(StreamWriteError, 0, "Failed to append data to stream.");
    }
}

template <class T>
void OutputStream::appendNoThrow(T&& aData) {
    try {
        OutputStreamExtender extender{SELF};
        extender << std::forward<T>(aData);
    } catch (const std::exception& aEx) {
        _logAppendingError(aEx.what());
        _setNotGood();
    }
}

template <class T>
auto OutputStream::operator<<(T&& aData)
    -> std::enable_if_t<supports_appending_of<T>::value, OutputStream&>
//
{
    OutputStreamExtender extender{SELF};
    extender << std::forward<T>(aData);
    if (hasWriteError()) {
        HG_THROW_TRACED(StreamWriteError, 0, "Failed to append data to stream.");
    }
    return SELF;
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_STREAM_OUTPUT_HPP
