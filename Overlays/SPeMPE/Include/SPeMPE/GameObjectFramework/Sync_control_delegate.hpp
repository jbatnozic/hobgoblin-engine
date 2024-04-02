#ifndef SPEMPE_GAME_OBJECT_FRAMEWORK_SYNC_CONTROL_DELEGATE_HPP
#define SPEMPE_GAME_OBJECT_FRAMEWORK_SYNC_CONTROL_DELEGATE_HPP

#include "Hobgoblin/Common/Positive_or_zero_integer.hpp"
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/GSL/HG_adapters.hpp>
#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/Utility/Packet.hpp>

#include <SPeMPE/GameObjectFramework/Sync_flags.hpp>

#include <cstdint>
#include <memory>
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
} // namespace detail

enum class SyncFilterStatus {
    REGULAR_SYNC   = 10, //! Sync whole state of this object (default behaviour)
    SKIP           = 20, //! Don't send anything during this update (no change)
    DEACTIVATE     = 30, //! Don't send anything and deactivate the remote dummy

    //! Has a priority of 0, meaning it will never be selected. It can be used
    //! together with filter() to get the currently set status.
    //! \warning FOR INTERNAL ENGINE USE ONLY
    __spempeimpl_ZERO = 0,

    //! Similar to REGULAR_SYNC but will force even Autodiff states to send
    //! full states and not just diffs.
    //! \warning FOR INTERNAL ENGINE USE ONLY
    __spempeimpl_FULL_STATE_SYNC = 5,

    //! Skip the sync on account of an Autodiff state without any changes.
    //! \warning FOR INTERNAL ENGINE USE ONLY
    __spempeimpl_SKIP_NO_DIFF = 15,

    //! \warning FOR INTERNAL ENGINE USE ONLY
    __spempeimpl_UNDEFINED = 99
};

//! Allows users to control how their synchronized objects are synced to
//! individual clients.
class SyncControlDelegate {
public:
    //! \returns The node through which the sync messages need to be sent.
    hg::RN_NodeInterface& getLocalNode() const;

    //! \returns Flags indicating the mode for the current sync.
    SyncFlags getSyncFlags() const;

    //! \returns Vector of client indices of recepients to which sync messages
    //!          need to be sent.
    const std::vector<hg::PZInteger>& getAllRecepients() const;

    //! \brief Determine to which clients (and how) to send state updates/syncs.
    //!
    //! \param aPredicate - Function which takes a PZInteger (client index) as
    //!                     a parameter and returns a SyncFilterStatus (one of
    //!                     REGULAR_SYNC, SKIP or DEACTIVATE).
    //!
    //! \warning Do not use the return value of this function, it's for the
    //!          engine's internal use only!
    //!
    //! \see filter
    int fnfilter(const std::function<SyncFilterStatus(hg::PZInteger)>& aPredicate);

    //! \brief same as fnfilter() but in template form (recommended to use this one).
    //!
    //! \see fnfilter
    template <class taCallable>
    int filter(taCallable&& aCallable);

    //! \returns Vector of client indices of recepients to which sync messages
    //!          need to be sent, to which filtering didn't set SKIP or DEACTIVATE.
    const std::vector<hg::PZInteger>& getFilteredRecepients() const;

private:
    friend class detail::SynchronizedObjectRegistry;

    SyncControlDelegate();
    ~SyncControlDelegate();

    class Impl;
    std::unique_ptr<Impl> _impl;

    int _applyFilterStatus(std::size_t aIndex, SyncFilterStatus aStatus);
};

inline
int SyncControlDelegate::fnfilter(
    const std::function<SyncFilterStatus(hg::PZInteger)>& aPredicate
) {
    return filter(aPredicate);    
}

template <class taCallable>
int SyncControlDelegate::filter(taCallable&& aCallable) {
    auto result = static_cast<int>(SyncFilterStatus::__spempeimpl_UNDEFINED);
    const auto& allRecepients = getAllRecepients();
    for (std::size_t i = 0; i < allRecepients.size(); i += 1) {
        const auto applied = _applyFilterStatus(i, aCallable(allRecepients[i]));
        if (applied < result) {
            result = applied;
        }
    }
    return result;
}

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_GAME_OBJECT_FRAMEWORK_SYNC_CONTROL_DELEGATE_HPP

