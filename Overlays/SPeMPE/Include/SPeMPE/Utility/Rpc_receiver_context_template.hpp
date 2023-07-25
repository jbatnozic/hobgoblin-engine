#ifndef SPEMPE_UTILITY_RPC_RECEIVER_CONTEXT_TEMPLATE_HPP
#define SPEMPE_UTILITY_RPC_RECEIVER_CONTEXT_TEMPLATE_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet.hpp>
#include <SPeMPE/GameContext/Game_context.hpp>

#include <chrono>

namespace jbatnozic {
namespace spempe {

namespace hg = ::jbatnozic::hobgoblin;

//! This class can be instantiated within the body of a RigelNet RPC (=within a handler)
//! to get easy access to the GameContext of the receiver and other relevant important info.
template <class taNetwMgr>
struct RPCReceiverContextTemplate {
    //! Reference to game context.
    GameContext& gameContext;

    //! Reference to instance of spempe::NetworkingManagerInterface.
    taNetwMgr& netwMgr;

    //! Index of the sender (always -1000 on client).
    int senderIndex;

    //! Latency to the sender (single direction, not round-trip).
    //! In case of a lag spike after which many packets arrive at once,
    //! this value should give you roughly something between the
    //! 'optimistic' and 'pessimistic' latencies (see below).
    //! During normal operation with a stable connection, all 3 latencies
    //! should have similar values.
    std::chrono::microseconds meanLatency;

    //! Latency to the sender (single direction, not round-trip).
    //! In case of a lag spike after which many packets arrive at once,
    //! the 'optimistic' latency will take into account only the most
    //! recent packet, so it shouldn't spike much (if at all).
    //! During normal operation with a stable connection, all 3 latencies
    //! should have similar values.
    std::chrono::microseconds optimisticLatency;

    //! Latency to the sender (single direction, not round-trip).
    //! In case of a lag spike after which many packets arrive at once,
    //! the 'pessimistic' latency will also briefly spike.
    //! During normal operation with a stable connection, all 3 latencies
    //! should have similar values.
    std::chrono::microseconds pessimisticLatency;

    //! Mean latency to the sender (single direction, not round-trip)
    //! expressed in steps (approximated using the desired framerate
    //! in the game context's runtime configuration).
    hg::PZInteger meanLatencyInSteps;

    //! Optimistic latency to the sender (single direction, not round-trip)
    //! expressed in steps (approximated using the desired framerate
    //! in the game context's runtime configuration).
    hg::PZInteger optimisticLatencyInSteps;

    //! Pessimistic latency to the sender (single direction, not round-trip)
    //! expressed in steps (approximated using the desired framerate
    //! in the game context's runtime configuration).
    hg::PZInteger pessimisticLatencyInSteps;

#define ROUND2INT(_x_) (static_cast<int>(std::round(_x_)))

    explicit RPCReceiverContextTemplate(const hg::RN_ClientInterface& aClient)
        : gameContext{*aClient.getUserDataOrThrow<GameContext>()}
        , netwMgr{gameContext.template getComponent<taNetwMgr>()}
        , senderIndex{-1000}
        , meanLatency{aClient.getServerConnector().getRemoteInfo().meanLatency / 2}
        , optimisticLatency{aClient.getServerConnector().getRemoteInfo().optimisticLatency / 2}
        , pessimisticLatency{aClient.getServerConnector().getRemoteInfo().pessimisticLatency / 2}
        , meanLatencyInSteps{ROUND2INT(meanLatency / gameContext.getRuntimeConfig().deltaTime)}
        , optimisticLatencyInSteps{ROUND2INT(optimisticLatency / gameContext.getRuntimeConfig().deltaTime)}
        , pessimisticLatencyInSteps{ROUND2INT(pessimisticLatency / gameContext.getRuntimeConfig().deltaTime)}
    {
    }

    explicit RPCReceiverContextTemplate(const hg::RN_ServerInterface& aServer)
        : gameContext{*aServer.getUserDataOrThrow<GameContext>()}
        , netwMgr{gameContext.template getComponent<taNetwMgr>()}
        , senderIndex{aServer.getSenderIndex()}
        , meanLatency{aServer.getClientConnector(senderIndex).getRemoteInfo().meanLatency / 2}
        , optimisticLatency{aServer.getClientConnector(senderIndex).getRemoteInfo().optimisticLatency / 2}
        , pessimisticLatency{aServer.getClientConnector(senderIndex).getRemoteInfo().pessimisticLatency / 2}
        , meanLatencyInSteps{ROUND2INT(meanLatency / gameContext.getRuntimeConfig().deltaTime)}
        , optimisticLatencyInSteps{ROUND2INT(optimisticLatency / gameContext.getRuntimeConfig().deltaTime)}
        , pessimisticLatencyInSteps{ROUND2INT(pessimisticLatency / gameContext.getRuntimeConfig().deltaTime)}
    {
    }

#undef ROUND2INT
};

namespace detail {

template <class taNetwMgr>
RPCReceiverContextTemplate<taNetwMgr> GetRPCReceiverContext(hg::RN_ClientInterface& aClient) {
    return RPCReceiverContextTemplate<taNetwMgr>{aClient};
}

template <class taNetwMgr>
RPCReceiverContextTemplate<taNetwMgr> GetRPCReceiverContext(hg::RN_ServerInterface& aServer) {
    return RPCReceiverContextTemplate<taNetwMgr>{aServer};
}

} // namespace detail

//! Creates an instance of RPCReceiverContext from a reference to either a
//! RN_ServerInterface or a RN_ClientInterface.
//! 
//! Note: This macro is intended for use by the engine itself (and when you use it, make sure
//! to #include <SPeMPE/Managers/Networking_manager_interface.hpp>). In user code, it's 
//! recommended to use the non-templated class `RPCReceiverContext` from
//! <SPeMPE/Utility/Rpc_receiver_context_user.hpp>.
#define SPEMPE_GET_RPC_RECEIVER_CONTEXT(_node_) \
    (::jbatnozic::spempe::detail::GetRPCReceiverContext<::jbatnozic::spempe::NetworkingManagerInterface>(_node_))

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_UTILITY_RPC_RECEIVER_CONTEXT_TEMPLATE_HPP
