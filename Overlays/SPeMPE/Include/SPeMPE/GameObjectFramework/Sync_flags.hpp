#ifndef SPEMPE_GAME_OBJECT_FRAMEWORK_SYNC_FLAGS_HPP
#define SPEMPE_GAME_OBJECT_FRAMEWORK_SYNC_FLAGS_HPP

#include <Hobgoblin/Utility/Packet.hpp>

#include <cstdint>

namespace jbatnozic {
namespace spempe {

namespace hg = ::jbatnozic::hobgoblin;

namespace detail {
//! Size is important because SyncFlags gets sent with every update sync RPC.
using SyncFlagsUnderlyingType = std::uint8_t;
} // namespace detail

enum class SyncFlags : detail::SyncFlagsUnderlyingType {
    NONE = 0x00,

    //! Shows whether the sync does/will contain the full state of the
    //! object (if not, only the diff since the last cycle will is/wil 
    //! be contained).
    FULL_STATE = 0x01,

    //! When set, this flag will force the receiving client to apply the
    //! update without chaining blue states in the state scheduler. This
    //! information is mostly only useful to the engine itself.
    NO_CHAIN = 0x40,

    //! Shows whether this update will be used as a pacemaker pulse on
    //! the client side. This information is mostly only useful to the
    //! engine itself.
    PACEMAKER_PULSE = 0x80,
};

//! Bitwise OR operator.
SyncFlags operator|(SyncFlags aLhs, SyncFlags aRhs);

//! Bitwise OR assignment operator.
SyncFlags& operator|=(SyncFlags& aLhs, SyncFlags aRhs);

//! Bitwise AND operator.
SyncFlags operator&(SyncFlags aLhs, SyncFlags aRhs);

//! Bitwise AND assignment operator.
SyncFlags& operator&=(SyncFlags& aLhs, SyncFlags aRhs);

//! Returns `true` if the `FULL_STATE` bit is set in `aFlags`, `false` otherwise.
bool IsFullStateFlagSet(SyncFlags aFlags);

//! Returns `true` if the `NO_CHAIN` bit is set in `aFlags`, `false` otherwise.
bool IsNoChainFlagSet(SyncFlags aFlags);

//! Returns `true` if the `PACEMAKER_PULSE` bit is set in `aFlags`, `false` otherwise.
bool IsPacemakerPulseFlagSet(SyncFlags aFlags);

//! Packing operator.
hg::util::Packet& operator<<(hg::util::PacketExtender& aPacket, SyncFlags aFlags);

//! Unpacking operator.
hg::util::Packet& operator>>(hg::util::PacketExtender& aPacket, SyncFlags& aFlags);

///////////////////////////////////////////////////////////////////////////
// INLINE IMPLEMENTATIONS                                                //
///////////////////////////////////////////////////////////////////////////

inline
SyncFlags operator|(SyncFlags aLhs, SyncFlags aRhs) {
    return static_cast<SyncFlags>(
        static_cast<detail::SyncFlagsUnderlyingType>(aLhs) | 
        static_cast<detail::SyncFlagsUnderlyingType>(aRhs)
    );
}

inline
SyncFlags& operator|=(SyncFlags& aLhs, SyncFlags aRhs) {
    return aLhs = static_cast<SyncFlags>(
        static_cast<detail::SyncFlagsUnderlyingType>(aLhs) | 
        static_cast<detail::SyncFlagsUnderlyingType>(aRhs)
    );
}

inline
SyncFlags operator&(SyncFlags aLhs, SyncFlags aRhs) {
    return static_cast<SyncFlags>(
        static_cast<detail::SyncFlagsUnderlyingType>(aLhs) &
        static_cast<detail::SyncFlagsUnderlyingType>(aRhs)
    );
}

inline
SyncFlags& operator&=(SyncFlags& aLhs, SyncFlags aRhs) {
    return aLhs = static_cast<SyncFlags>(
        static_cast<detail::SyncFlagsUnderlyingType>(aLhs) &
        static_cast<detail::SyncFlagsUnderlyingType>(aRhs)
    );
}

inline
bool IsFullStateFlagSet(SyncFlags aFlags) {
    return ((aFlags & SyncFlags::FULL_STATE) != SyncFlags::NONE);
}

inline
bool IsNoChainFlagSet(SyncFlags aFlags) {
    return ((aFlags & SyncFlags::NO_CHAIN) != SyncFlags::NONE);
}

inline
bool IsPacemakerPulseFlagSet(SyncFlags aFlags) {
    return ((aFlags & SyncFlags::PACEMAKER_PULSE) != SyncFlags::NONE);
}

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_GAME_OBJECT_FRAMEWORK_SYNC_FLAGS_HPP

