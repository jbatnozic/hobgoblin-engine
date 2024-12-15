// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <SPeMPE/GameObjectFramework/Sync_flags.hpp>

namespace jbatnozic {
namespace spempe {

hg::util::OutputStream& operator<<(hg::util::OutputStreamExtender& aOStream, SyncFlags aFlags) {
    return (aOStream << static_cast<detail::SyncFlagsUnderlyingType>(aFlags));
}

hg::util::InputStream& operator>>(hg::util::InputStreamExtender& aIStream, SyncFlags& aFlags) {
    const auto value = aIStream->extractNoThrow<detail::SyncFlagsUnderlyingType>();
    aFlags = static_cast<SyncFlags>(value);
    return *aIStream;
}

} // namespace spempe
} // namespace jbatnozic

// clang-format on
