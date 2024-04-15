// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef SPEMPE_GAME_OBJECT_FRAMEWORK_DEFAULT_SYNC_IMPL_HPP
#define SPEMPE_GAME_OBJECT_FRAMEWORK_DEFAULT_SYNC_IMPL_HPP

#include <Hobgoblin/QAO.hpp>
#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/RigelNet_macros.hpp>
#include <SPeMPE/GameContext/Game_context.hpp>
#include <SPeMPE/GameObjectFramework/Autodiff_state.hpp>
#include <SPeMPE/GameObjectFramework/Synchronized_object_registry.hpp>
#include <SPeMPE/GameObjectFramework/Sync_control_delegate.hpp>
#include <SPeMPE/GameObjectFramework/Sync_id.hpp>
#include <SPeMPE/Managers/Networking_manager_interface.hpp>
#include <SPeMPE/Utility/Rpc_receiver_context_template.hpp>

#include <type_traits>

#include <Hobgoblin/Logging.hpp>

namespace jbatnozic {
namespace spempe {

namespace hg = ::jbatnozic::hobgoblin;

namespace detail {

template <class taSyncObj, class taNetwMgr>
void DefaultSyncCreateHandler(hg::RN_NodeInterface& node, SyncId syncId) {
    node.callIfClient([&](hg::RN_ClientInterface& client) {
        auto& ctx        = *client.getUserDataOrThrow<GameContext>();
        auto& runtime    = ctx.getQAORuntime();
        auto  regId      = ctx.getComponent<NetworkingManagerInterface>().getRegistryId();
        auto& syncObjReg = *reinterpret_cast<detail::SynchronizedObjectRegistry*>(regId.address);

        if (syncObjReg.getMapping(syncId) == nullptr) {
            hg::QAO_PCreate<taSyncObj>(&runtime, regId, syncId);
        }
    });

    node.callIfServer([](hg::RN_ServerInterface&) {
        throw hg::RN_IllegalMessage("Server received a sync message");
    });
}

template <class taSyncObj, class taNetwMgr>
void DefaultSyncUpdateHandler(
    hg::RN_NodeInterface& node,
    SyncId syncId,
    SyncFlags flags,
    typename taSyncObj::VisibleState& state)
{
    node.callIfClient([&](hg::RN_ClientInterface& client) {
        auto  rc         = SPEMPE_GET_RPC_RECEIVER_CONTEXT(client);
        auto  regId      = rc.netwMgr.getRegistryId();
        auto& syncObjReg = *reinterpret_cast<detail::SynchronizedObjectRegistry*>(regId.address);
        auto* object     = static_cast<taSyncObj*>(syncObjReg.getMapping(syncId));

        if (object) {
            object->__spempeimpl_applyUpdate(
                state,
                syncObjReg.adjustDelayForLag(rc.pessimisticLatencyInSteps),
                flags
            );
        }
    });

    node.callIfServer([](hg::RN_ServerInterface&) {
        throw hg::RN_IllegalMessage("Server received a sync message");
    });
}

template <class taSyncObj, class taNetwMgr>
void DefaultSyncDestroyHandler(hg::RN_NodeInterface& node, SyncId syncId) {
    node.callIfClient([&](hg::RN_ClientInterface& client) {
        auto  rc         = SPEMPE_GET_RPC_RECEIVER_CONTEXT(client);
        auto  regId      = rc.gameContext.template getComponent<taNetwMgr>().getRegistryId();
        auto& syncObjReg = *reinterpret_cast<detail::SynchronizedObjectRegistry*>(regId.address);
        auto* object     = static_cast<taSyncObj*>(syncObjReg.getMapping(syncId));

        if (object) {
            object->__spempeimpl_destroySelfIn(
                static_cast<int>(syncObjReg.getDefaultDelay())
                - (syncObjReg.adjustDelayForLag(rc.meanLatencyInSteps) + 1)
            );
        }
    });

    node.callIfServer([](hg::RN_ServerInterface&) {
        throw hg::RN_IllegalMessage("Server received a sync message");
    });
}

//! Returns `true` if the referenced object was previously skipped or 
//! deactivated for any of the clients to which we intend to send updates.
template <class taSynchronizedObject>
bool WasPreviouslySkippedOrDeactivated(const taSynchronizedObject& aObject,
                                       const SyncControlDelegate& aSyncCtrl) {
    for (const auto client : aSyncCtrl.getFilteredRecepients()) {
        if (aObject.__spempeimpl_getNoDiffSkipFlagForClient(client) ||
            aObject.__spempeimpl_getSkipFlagForClient(client) ||
            aObject.__spempeimpl_getDeactivationFlagForClient(client)) {
            return true;
        }
    }
    return false;
}

inline
SyncFilterStatus GetLowestSetFilterStatus(SyncControlDelegate& aSyncCtrl) {
    // This is a trick to sniff out the current status because ZERO won't ever
    // be selected over any other status.
    const auto result = aSyncCtrl.filter(
        [](hg::PZInteger /* aClientIndex */) -> SyncFilterStatus {
            return detail::SyncFilterStatus_ZERO;
        });
    return static_cast<SyncFilterStatus>(result);
}

//! Align the configuration of visible state and of the sync details before sending.
//! 
//! \param aObject Synchronized object currently being synced.
//! \param aVisibleState Current visible state of the synchronized object.
//! \param aSyncCtrl SyncControlDelegate for the current sync.
//! \param aSyncFlags Pointer to a variable of type `SyncFlags`. This function will
//!                   fill this variable with a `SyncFlags` value that MUST be used
//!                   for the sync to follow.
template <class taSynchronizedObject, class taVisisbleState>
AutodiffPackMode AlignState_PreCompose(const taSynchronizedObject& aObject,
                                       taVisisbleState& aVisibleState,
                                       SyncControlDelegate& aSyncCtrl,
                                       SyncFlags* aSyncFlags) {
    *aSyncFlags = aSyncCtrl.getSyncFlags();

    // REGULAR STATE
    if constexpr (!std::is_base_of<AutodiffStateTag, taVisisbleState>::value) {
        if (WasPreviouslySkippedOrDeactivated(aObject, aSyncCtrl)) {
            *aSyncFlags |= SyncFlags::NO_CHAIN;
        }
        // The specific return value doesn't matter here
        return AutodiffPackMode::Default;
    }

    // AUTODIFF STATE
    if constexpr (std::is_base_of<AutodiffStateTag, taVisisbleState>::value) {
        const auto previousPackMode = aVisibleState.getPackMode();

        if (IsFullStateFlagSet(*aSyncFlags)) {
            // If a full state sync was requested by the engine (to initialize
            // a newly connected client), we must not filter out any recepients
            // nor allow only the diff to be sent.
            aVisibleState.setPackMode(AutodiffPackMode::PackAll);
            *aSyncFlags |= SyncFlags::NO_CHAIN;
            return previousPackMode;
        }

        auto lowestStatus = detail::SyncFilterStatus_UNDEFINED;
        if (aVisibleState.cmp() == AUTODIFF_STATE_NO_CHANGE &&
            aVisibleState.getNoChangeStreakCount() >= 1) {
            const auto result = aSyncCtrl.filter(
                [](hg::PZInteger /* aClientIndex */) -> SyncFilterStatus {
                    return detail::SyncFilterStatus_SKIP_NO_DIFF;
                });
            lowestStatus = static_cast<SyncFilterStatus>(result);
        } else {
            lowestStatus = GetLowestSetFilterStatus(aSyncCtrl);
        }

        switch (lowestStatus) {
        case detail::SyncFilterStatus_RESUMING_SYNC:
            aVisibleState.setPackMode(AutodiffPackMode::PackAll);
            // We already know from the result of filter() that the object was
            // previously skipped or deactivated for at least one client.
            *aSyncFlags |= SyncFlags::NO_CHAIN;
            break;

        case SyncFilterStatus::REGULAR_SYNC:
            if (WasPreviouslySkippedOrDeactivated(aObject, aSyncCtrl)) {
                aVisibleState.setPackMode(AutodiffPackMode::PackAll);
                *aSyncFlags |= SyncFlags::NO_CHAIN;
            } else {
                aVisibleState.setPackMode(AutodiffPackMode::PackDiff);
            }
            break;
        
        default:
            // Do nothing
            break;
        }

        return previousPackMode;
    }
}

//! Align the configuration of visible state and of the sync details after sending.
//! (puts everything back the way it was)
//! Note: this only does something if the state is an autodiff state.
template <class taAutodiffState>
void AlignState_PostCompose(taAutodiffState& aAutodiffState, AutodiffPackMode aOldPackMode) {
    if constexpr (std::is_base_of<AutodiffStateTag, taAutodiffState>::value) {
        aAutodiffState.setPackMode(aOldPackMode);
    }
}

//! `_syncUpdateImpl` is a const method, but if the visible state is an autodiff state, we may
//! need to alter the pack mode. That's why we need to const_cast away the constness from the current
//! state in order to pass it to AlignState_* functions. This isn't UB because the pack mode variable
//! inside autopack states is mutable, and because these are const references to non-const objects
//! anyway.
template <class T>
T& StripConstFromRef(const T& aRef) {
    return const_cast<T&>(aRef);
}

} // namespace detail

// ===== Internal begin =====

#define USPEMPE_MACRO_EXPAND(_x_) _x_
#ifndef USPEMPE_MACRO_EXPAND_VA
#define USPEMPE_MACRO_EXPAND_VA(...) __VA_ARGS__
#endif // !USPEMPE_MACRO_EXPAND_VA

#define USPEMPE_GENERATE_DEFAULT_SYNC_HANDLER_EMPTY(_class_name_) /* Empty */

#define USPEMPE_GENERATE_DEFAULT_SYNC_HANDLER_CREATE(_class_name_) \
    RN_DEFINE_RPC(USPEMPE_Create##_class_name_, \
                  RN_ARGS(::jbatnozic::spempe::SyncId, syncId)) { \
        ::jbatnozic::spempe::detail::DefaultSyncCreateHandler<_class_name_, \
                                                              ::jbatnozic::spempe::NetworkingManagerInterface>( \
            RN_NODE_IN_HANDLER(), syncId); \
    }

#define USPEMPE_GENERATE_DEFAULT_SYNC_HANDLER_UPDATE(_class_name_) \
    RN_DEFINE_RPC(USPEMPE_Update##_class_name_, \
                  RN_ARGS(::jbatnozic::spempe::SyncId, syncId, \
                          ::jbatnozic::spempe::SyncFlags, flags, \
                          _class_name_::VisibleState&, state)) { \
        ::jbatnozic::spempe::detail::DefaultSyncUpdateHandler<_class_name_, \
                                                              ::jbatnozic::spempe::NetworkingManagerInterface>( \
            RN_NODE_IN_HANDLER(), syncId, flags, state); \
    }

#define USPEMPE_GENERATE_DEFAULT_SYNC_HANDLER_DESTROY(_class_name_) \
    RN_DEFINE_RPC(USPEMPE_Destroy##_class_name_, \
                  RN_ARGS(::jbatnozic::spempe::SyncId, syncId)) { \
        ::jbatnozic::spempe::detail::DefaultSyncDestroyHandler<_class_name_, \
                                                               ::jbatnozic::spempe::NetworkingManagerInterface>( \
            RN_NODE_IN_HANDLER(), syncId); \
    }

#define USPEMPE_GENERATE_MULTIPLE_DEFAULT_SYNC_HANDLERS(_class_name_, _tag_1_, _tag_2_, _tag_3_, ...) \
    USPEMPE_GENERATE_DEFAULT_SYNC_HANDLER_##_tag_1_ (_class_name_) \
    USPEMPE_GENERATE_DEFAULT_SYNC_HANDLER_##_tag_2_ (_class_name_) \
    USPEMPE_GENERATE_DEFAULT_SYNC_HANDLER_##_tag_3_ (_class_name_)

#define USPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(_class_name_, ...) \
    USPEMPE_MACRO_EXPAND( \
        USPEMPE_GENERATE_MULTIPLE_DEFAULT_SYNC_HANDLERS(_class_name_, __VA_ARGS__, EMPTY, EMPTY) \
    )

// ===== Internal end =====

//! Create default synchronization handlers for class _class_name_.
//! Events should be written together in parenthesis and can be CREATE, UPDATE and DESTROY.
//! Example of usage:
//!     SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(MyClass, (CREATE, DESTROY));
#define SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(_class_name_, _for_events_) \
    USPEMPE_MACRO_EXPAND( \
        USPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(_class_name_, USPEMPE_MACRO_EXPAND_VA _for_events_) \
    )

//! Calls the default sync/CREATE implementation of a SynchronizedObject
//! generated by SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS.
#define SPEMPE_SYNC_CREATE_DEFAULT_IMPL(_class_name_, _sync_ctrl_) \
    Compose_USPEMPE_Create##_class_name_((_sync_ctrl_).getLocalNode(), \
                                         (_sync_ctrl_).getFilteredRecepients(), \
                                         this->getSyncId())

//! Calls the default sync/UPDATE implementation of a SynchronizedObject
//! generated by SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS.
//! Note: Will properly detect and handle autodiff objects.
#define SPEMPE_SYNC_UPDATE_DEFAULT_IMPL(_class_name_, _sync_ctrl_) \
    do { \
        ::jbatnozic::spempe::SyncFlags __spempeImpl_localSyncFlags; \
        const auto __spempeimpl_oldPackMode = ::jbatnozic::spempe::detail::AlignState_PreCompose( \
            *this, \
            ::jbatnozic::spempe::detail::StripConstFromRef(_getCurrentState()), \
            _sync_ctrl_, \
            &__spempeImpl_localSyncFlags \
        ); \
        Compose_USPEMPE_Update##_class_name_((_sync_ctrl_).getLocalNode(), \
                                             (_sync_ctrl_).getFilteredRecepients(), \
                                             this->getSyncId(), \
                                             __spempeImpl_localSyncFlags, \
                                             _getCurrentState()); \
        ::jbatnozic::spempe::detail::AlignState_PostCompose( \
            ::jbatnozic::spempe::detail::StripConstFromRef(_getCurrentState()), \
            __spempeimpl_oldPackMode \
        ); \
    } while (false)

//! Calls the default sync/DESTROY implementation of a SynchronizedObject
//! generated by SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS.
#define SPEMPE_SYNC_DESTROY_DEFAULT_IMPL(_class_name_, _sync_ctrl_) \
    Compose_USPEMPE_Destroy##_class_name_((_sync_ctrl_).getLocalNode(), \
                                          (_sync_ctrl_).getFilteredRecepients(), \
                                          this->getSyncId())

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_GAME_OBJECT_FRAMEWORK_DEFAULT_SYNC_IMPL_HPP

// clang-format on
