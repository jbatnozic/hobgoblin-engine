// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_BASE_64_HPP
#define UHOBGOBLIN_UTIL_BASE_64_HPP

#include <Hobgoblin/Common.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

//! Determines the minimum amount of space needed to base-64-encode a buffer of binary data.
//! \param aInputBufferByteCount size (in bytes) of the input buffer to encode.
//! \returns minimum size of the output buffer (roughly 133% of the size of the input buffer).
PZInteger GetRecommendedOutputBufferSizeForBase64Encode(PZInteger aInputBufferByteCount);

//! TODO(description)
PZInteger Base64Encode(
    /*  in */ const void* aInputBuffer,
    /*  in */ PZInteger   aInputBufferByteCount,
    /* out */ void*       aOutputBuffer,
    /*  in */ PZInteger   aOutputBufferByteCount);

//! Determines the minimum amount of space needed to base-64-decode a buffer of binary data.
//! \param aInputBufferByteCount size (in bytes) of the input buffer to decode.
//! \returns minimum size of the output buffer (roughly 75% of the size of the input buffer).
PZInteger GetRecommendedOutputBufferSizeForBase64Decode(PZInteger aInputBufferByteCount);

//! TODO(description)
PZInteger Base64Decode(
    /*  in */ const void* aInputBuffer,
    /*  in */ PZInteger   aInputBufferByteCount,
    /* out */ void*       aOutputBuffer,
    /*  in */ PZInteger   aOutputBufferByteCount);

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_BASE_64_HPP
