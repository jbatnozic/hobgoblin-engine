// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <SPeMPE/GameObjectFramework/Sync_flags.hpp>

namespace jbatnozic {
namespace spempe {

hg::util::Packet& operator<<(hg::util::PacketExtender& aPacket, SyncFlags aFlags) {
    return (aPacket << static_cast<detail::SyncFlagsUnderlyingType>(aFlags));
}

hg::util::Packet& operator>>(hg::util::PacketExtender& aPacket, SyncFlags& aFlags) {
    const auto value = aPacket->extractNoThrow<detail::SyncFlagsUnderlyingType>();
    aFlags = static_cast<SyncFlags>(value);
    return *aPacket;
}

} // namespace spempe
} // namespace jbatnozic

// clang-format on
