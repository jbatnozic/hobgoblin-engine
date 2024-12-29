// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Utility/Base64.hpp>

#include <libbase64.h>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

PZInteger GetRecommendedOutputBufferSizeForBase64Encode(PZInteger aInputBufferByteCount) {
    return ((aInputBufferByteCount * 4 + 8) / 3);
}

PZInteger Base64Encode(NeverNull<const void*> aInputBuffer,
                       PZInteger              aInputBufferByteCount,
                       NeverNull<void*>       aOutputBuffer,
                       PZInteger              aOutputBufferByteCount) {
    if (aInputBufferByteCount == 0) {
        return 0;
    }

    HG_VALIDATE_ARGUMENT(aOutputBufferByteCount >=
                         GetRecommendedOutputBufferSizeForBase64Encode(aInputBufferByteCount));

    std::size_t outlen;
    base64_encode(static_cast<const char*>(aInputBuffer.get()),
                  pztos(aInputBufferByteCount),
                  static_cast<char*>(aOutputBuffer.get()),
                  &outlen,
                  0);

    HG_ASSERT(stopz(outlen) <= aOutputBufferByteCount);

    return stopz(outlen);
}

PZInteger GetRecommendedOutputBufferSizeForBase64Decode(PZInteger aInputBufferByteCount) {
    return ((aInputBufferByteCount * 3 + 11) / 4);
}

PZInteger Base64Decode(NeverNull<const void*> aInputBuffer,
                       PZInteger              aInputBufferByteCount,
                       NeverNull<void*>       aOutputBuffer,
                       PZInteger              aOutputBufferByteCount) {
    if (aInputBufferByteCount == 0) {
        return 0;
    }

    HG_VALIDATE_ARGUMENT(aOutputBufferByteCount >=
                         GetRecommendedOutputBufferSizeForBase64Decode(aInputBufferByteCount));

    std::size_t outlen;
    base64_decode(static_cast<const char*>(aInputBuffer.get()),
                  pztos(aInputBufferByteCount),
                  static_cast<char*>(aOutputBuffer.get()),
                  &outlen,
                  0);

    HG_ASSERT(stopz(outlen) <= aOutputBufferByteCount);

    return stopz(outlen);
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
