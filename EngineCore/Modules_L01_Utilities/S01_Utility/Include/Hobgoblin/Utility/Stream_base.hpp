// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_STREAM_BASE_HPP
#define UHOBGOBLIN_UTIL_STREAM_BASE_HPP

#include <cstdint>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

class StreamBase {
public:
    //! Error code indicating that the operation failed.
    static constexpr std::int64_t E_FAILURE = -1;
    
    //! Error code indicating that the operation failed AND that the stream has reached an
    //! invalid state where no further I/O operations are possible. This error code can
    //! also be returned if the stream was already in an invalid state at the time that the
    //! operation was called.
    static constexpr std::int64_t E_BADSTATE = -3;

    //! Error code used for getters to indicate that the specific underlying implementation
    //! of the stream has no information about the query, or the query has no meaning for
    //! this implementation.
    static constexpr std::int64_t E_UNKNOWN = -2;
};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_STREAM_BASE_HPP
