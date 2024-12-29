// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_NOTHROW_HPP
#define UHOBGOBLIN_UTIL_NOTHROW_HPP

#include <Hobgoblin/Common/Strong_bool.hpp>
#include <Hobgoblin/Utility/Stream_input.hpp>
#include <Hobgoblin/Utility/Stream_output.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

//! \see comment for `HG_DISAMBIGUATE_NOTHROW` macro below.
template <class T>
class FreeNoThrowAdapter {
public:
    // clang-format off
    template <class T2,
              T_ENABLE_IF(OutputStream::supports_appending_of<T2&>::value)>
    FreeNoThrowAdapter& operator<<(T2& aRef) {
        _stream.appendNoThrow(aRef);
        return SELF;
    }
    // clang-format on

    // clang-format off
    template <class T2,
              T_ENABLE_IF(InputStream::supports_extracting_of<T2&>::value)>
    FreeNoThrowAdapter& operator>>(T2& aRef) {
        aRef = _stream.template extractNoThrow<T2>();
        return SELF;
    }
    // clang-format on

    operator StrongBool() const {
        return (_stream) ? SBOOL_TRUE : SBOOL_FALSE;
    }

private:
    friend T;

    FreeNoThrowAdapter(T& aStream)
        : _stream{aStream} {}

    T& _stream;
};

//! If a class inherits from both `OutputStream` and `InputStream`, we must
//! put this macro somewhere in the public part of its body; otherwise calls
//! to `.noThrow()` will be ambiguous between the two base classes.
//!
//! Example:
//! `struct Packet : OutputStream, InputStream { /*...*/ HG_DISAMBIGUATE_NOTHROW(Packet) };`
#define HG_DISAMBIGUATE_NOTHROW(_class_)    \
    FreeNoThrowAdapter<_class_> noThrow() { \
        return {*this};                     \
    }

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_NOTHROW_HPP
