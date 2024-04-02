#ifndef SPEMPE_GAME_OBJECT_FRAMEWORK_SYNC_CONTROL_DELEGATE_HPP
#define SPEMPE_GAME_OBJECT_FRAMEWORK_SYNC_CONTROL_DELEGATE_HPP

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

    __spempeimpl_FULL_STATE_SYNC = 5,
    __spempeimpl_SKIP_NO_DIFF = 15,
    __spempeimpl_UNDEFINED = 99
};

//! Allows users to control how their synchronized objects are synced to
//! individual clients.
class SyncControlDelegate {
public:
    //! The node through which the sync messages need to be sent.
    hg::RN_NodeInterface& getLocalNode() const;

    //! TODO
    SyncFlags getSyncFlags() const;

    //! Vector of client indices of (recepients) to which sync messages
    //! need to be sent.
    const std::vector<hg::PZInteger>& getAllRecepients() const;

    //! TODO
    const std::vector<hg::PZInteger>& getFilteredRecepients() const;

    //! TODO
    template <class taCallable>
    int filter(taCallable&& aCallable);

private:
    friend class detail::SynchronizedObjectRegistry;

    SyncControlDelegate();
    ~SyncControlDelegate();

    class Impl;
    std::unique_ptr<Impl> _impl;

    int _applyFilterStatus(std::size_t aIndex, SyncFilterStatus aStatus);
};

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

