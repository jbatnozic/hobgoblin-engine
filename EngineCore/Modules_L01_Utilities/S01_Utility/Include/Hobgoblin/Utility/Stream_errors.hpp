// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_STREAM_ERRORS_HPP
#define UHOBGOBLIN_UTIL_STREAM_ERRORS_HPP

#include <Hobgoblin/HGExcept.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

//! Traced exception that's thrown when writing data into an OutputStream fails.
class StreamWriteError : public TracedRuntimeError {
public:
    using TracedRuntimeError::TracedRuntimeError;
};

//! Traced exception that's thrown when reading data from an InputStream fails.
class StreamReadError : public TracedRuntimeError {
public:
    using TracedRuntimeError::TracedRuntimeError;
};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_STREAM_ERRORS_HPP
