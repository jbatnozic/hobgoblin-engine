// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_NOTHROW_HPP
#define UHOBGOBLIN_UTIL_NOTHROW_HPP

#include <Hobgoblin/Utility/Stream_bool.hpp>
#include <Hobgoblin/Utility/Stream_input.hpp>
#include <Hobgoblin/Utility/Stream_output.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

template <class T>
class FreeNoThrowAdapter {
public:
    // clang-format off
    template <class T,
        T_ENABLE_IF(OutputStream::supports_appending_of<T&>::value)>
    FreeNoThrowAdapter& operator<<(T& aRef) {
        _stream.append(aRef);
        return SELF;
    }
    // clang-format on

    // clang-format off
    template <class T,
              T_ENABLE_IF(InputStream::supports_extracting_of<T&>::value)>
    FreeNoThrowAdapter& operator>>(T& aRef) {
        aRef = _stream.extractNoThrow<T>();
        return SELF;
    }
    // clang-format on

    //! TODO
    operator StreamBool::BoolType() const {
        return (_stream) ? &StreamBool::dummy : nullptr;
    }

private:
    friend T;

    FreeNoThrowAdapter(T& aStream)
        : _stream{aStream} {}

    T& _stream;
};

#define HG_DISAMBIGUATE_NOTHROW(_class_)    \
    FreeNoThrowAdapter<_class_> noThrow() { \
        return {*this};                     \
    }

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_NOTHROW_HPP