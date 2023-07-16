
#include <SPeMPE/Other/Rpc_receiver_context.hpp>
#include <SPeMPE/GameObjectFramework/Sync_details.hpp>
#include <SPeMPE/GameObjectFramework/Synchronized_object_registry.hpp>
#include <SPeMPE/Other/Rpc_receiver_context.hpp>

#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/RigelNet_macros.hpp>

namespace jbatnozic {
namespace spempe {

namespace {
RN_DEFINE_RPC(USPEMPE_DeactivateObject, RN_ARGS(SyncId, aSyncId)) {
    RN_NODE_IN_HANDLER().callIfClient(
        [=](hg::RN_ClientInterface& client) {
        RPCReceiverContext rc{client};
        auto  regId      = rc.netwMgr.getRegistryId();
        auto& syncObjReg = *reinterpret_cast<detail::SynchronizedObjectRegistry*>(regId.address);

        syncObjReg.deactivateObject(aSyncId, rc.pessimisticLatencyInSteps);
    });

    RN_NODE_IN_HANDLER().callIfServer(
        [](hg::RN_ServerInterface&) {
        throw hg::RN_IllegalMessage("Server received a sync message");
    });
}
} // namespace

SyncDetails::SyncDetails(detail::SynchronizedObjectRegistry& aRegistry)
    : _registry{&aRegistry}
{
}

hg::RN_NodeInterface& SyncDetails::getNode() const {
    return _registry->getNode();
}

const std::vector<hg::PZInteger>& SyncDetails::getRecepients() const {
    return _recepients;
}

void SyncDetails::filterSyncs(const SyncDetails::FilterPrecidateFunc& aPredicate) {
    constexpr static bool REMOVE_IF_DO_REMOVE   = true;
    constexpr static bool REMOVE_IF_DONT_REMOVE = false;

    auto& reg = *_registry;
    const SyncId forObject = _forObject;

    auto iter = std::remove_if(_recepients.begin(), _recepients.end(),
        [&](hg::PZInteger aRecepient) -> bool {
        switch (aPredicate(aRecepient)) {
        case FilterResult::FullSync:
            return REMOVE_IF_DONT_REMOVE;

        case FilterResult::Skip:
            return REMOVE_IF_DO_REMOVE;

        case FilterResult::Deactivate:
            if (!reg.isObjectDeactivatedForClient(forObject, aRecepient)) {
                reg.setObjectDeactivatedFlagForClient(forObject, aRecepient, true);
                Compose_USPEMPE_DeactivateObject(reg.getNode(), aRecepient, forObject);
            }
            return REMOVE_IF_DO_REMOVE;

        default:
            assert(false && "Unreachable");
            break;
        }
    });

    _recepients.erase(iter, _recepients.end());
}

} // namespace spempe
} // namespace jbatnozic
