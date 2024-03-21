#ifndef SPEMPE_GAME_OBJECT_FRAMEWORK_SYNC_DETAILS_HPP
#define SPEMPE_GAME_OBJECT_FRAMEWORK_SYNC_DETAILS_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/GSL/HG_adapters.hpp>
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
//! Size is important because SyncFlags gets sent with every update sync RPC.
using SyncFlagsUnderlyingType = std::uint8_t;
} // namespace detail

enum class SyncFlags : detail::SyncFlagsUnderlyingType {
    None = 0x00,
    //! Shows whether this update will be used as a pacemaker pulse on
    //! the client side. This information is mostly only useful to the
    //! engine itself.
    PacemakerPulse = 0x01,
    //! Shows whether the sync does/will contain the full state of the
    //! object (if not, only the diff since the last cycle will is/wil 
    //! be contained).
    FullState = 0x02,
};

//! Bitwise OR operator.
SyncFlags operator|(SyncFlags aLhs, SyncFlags aRhs);

//! Bitwise OR assignment operator.
SyncFlags& operator|=(SyncFlags& aLhs, SyncFlags aRhs);

//! Bitwise AND operator.
SyncFlags operator&(SyncFlags aLhs, SyncFlags aRhs);

//! Bitwise AND assignment operator.
SyncFlags& operator&=(SyncFlags& aLhs, SyncFlags aRhs);

//! Returns `true` if the `PacemakerPulse` bit is set in `aFlags`, `false` otherwise.
bool HasPacemakerPulse(SyncFlags aFlags);

//! Returns `true` if the `FullState` bit is set in `aFlags`, `false` otherwise.
bool HasFullState(SyncFlags aFlags);

//! Packing operator.
hg::util::Packet& operator<<(hg::util::PacketExtender& aPacket, SyncFlags aFlags);

//! Unpacking operator.
hg::util::Packet& operator>>(hg::util::PacketExtender& aPacket, SyncFlags& aFlags);

namespace detail {
class SynchronizedObjectRegistry;
} // namespace detail

struct SyncDetails {
    //! The node through which the sync messages need to be sent.
    hg::RN_NodeInterface& getNode() const;

    //! Vector of client indices of (recepients) to which sync messages
    //! need to be sent.
    const std::vector<hg::PZInteger>& getRecepients() const;

    SyncFlags getFlags() const;

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

    hg::NotNull<detail::SynchronizedObjectRegistry*> _registry;
    std::vector<hg::PZInteger> _recepients;
    std::int64_t _qaoStepOrdinal;
    SyncId _forObject = SYNC_ID_NEW;
    SyncFlags _flags = SyncFlags::None;

    friend class detail::SynchronizedObjectRegistry;
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_GAME_OBJECT_FRAMEWORK_SYNC_DETAILS_HPP

