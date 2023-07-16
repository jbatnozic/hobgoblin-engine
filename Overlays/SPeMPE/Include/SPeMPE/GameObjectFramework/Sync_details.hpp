#ifndef SPEMPE_GAME_OBJECT_FRAMEWORK_SYNC_DETAILS_HPP
#define SPEMPE_GAME_OBJECT_FRAMEWORK_SYNC_DETAILS_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/Utility/Packet.hpp>

#include <cstdint>
#include <functional>
#include <vector>

namespace jbatnozic {
namespace spempe {

namespace hg = ::jbatnozic::hobgoblin;

//! Number identifying a SynchronizedObject.
using SyncId = std::uint64_t;

constexpr SyncId SYNC_ID_NEW = 0;

namespace detail {
class SynchronizedObjectRegistry;
using SyncFlagsUnderlyingType = std::uint8_t;
} // namespace detail

enum class SyncFlags : detail::SyncFlagsUnderlyingType {
    None = 0x00,
    //! Shows whether this update will be used as a pacemaker pulse on
    //! the client side. This information is mostly only useful to the
    //! engine itself.
    PacemakerPulse = 0x01,
    //! Shows whether it's allowed to send only the diff of this object
    //! since the last cycle.
    DiffAllowed = 0x02, // TODO: Invert -> FullState
};

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
bool HasPacemakerPulse(SyncFlags aFlags) {
    return ((aFlags & SyncFlags::PacemakerPulse) != SyncFlags::None);
}

inline
bool IsDiffAllowed(SyncFlags aFlags) {
    return ((aFlags & SyncFlags::DiffAllowed) != SyncFlags::None);
}

inline
hg::util::PacketBase& operator<<(hg::util::PacketBase& aPacket, SyncFlags aFlags) {
    return (aPacket << static_cast<detail::SyncFlagsUnderlyingType>(aFlags));
}

inline
hg::util::PacketBase& operator>>(hg::util::PacketBase& aPacket, SyncFlags& aFlags) {
    detail::SyncFlagsUnderlyingType value;
    aPacket >> value;
    aFlags = static_cast<SyncFlags>(value);
    return aPacket;
}

struct SyncDetails {
    //! The node through which the sync messages need to be sent.
    hg::RN_NodeInterface& getNode() const;

    //! Vector of client indices of (recepients) to which sync messages
    //! need to be sent.
    const std::vector<hg::PZInteger>& getRecepients() const;

    SyncFlags getFlags() {
        return _flags;
    }

    enum class FilterResult {
        FullSync,   //! Sync whole state of this object (default behaviour) TODO rename this
        Skip,       //! Don't send anything during this update (no change)
        Deactivate  //! Don't send anything and deactivate the remote dummy
    };

    //! Function object that takes an index of a client as an argument and should
    //! return whether (and how) to sync an object to this particular client.
    using FilterPrecidateFunc = std::function<FilterResult(hg::PZInteger)>;

    void filterSyncs(const FilterPrecidateFunc& aPredicate);

private:
    SyncDetails(detail::SynchronizedObjectRegistry& aRegistry);

    hg::not_null<detail::SynchronizedObjectRegistry*> _registry;
    std::vector<hg::PZInteger> _recepients;
    SyncId _forObject = SYNC_ID_NEW;
    SyncFlags _flags = SyncFlags::None;

    friend class detail::SynchronizedObjectRegistry;
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_GAME_OBJECT_FRAMEWORK_SYNC_DETAILS_HPP

