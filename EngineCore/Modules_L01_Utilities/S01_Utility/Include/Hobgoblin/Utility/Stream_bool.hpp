// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_STREAM_BOOL_HPP
#define UHOBGOBLIN_UTIL_STREAM_BOOL_HPP

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

class StreamBool {
public:
    using BoolType = void (StreamBool::*)();

    void dummy() {}
};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_STREAM_BOOL_HPP
