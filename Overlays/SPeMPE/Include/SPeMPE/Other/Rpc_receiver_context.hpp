#ifndef SPEMPE_OTHER_RPC_RECEIVER_CONTEXT_HPP
#define SPEMPE_OTHER_RPC_RECEIVER_CONTEXT_HPP

#include <SPeMPE/GameContext/Game_context.hpp>
#include <SPeMPE/Managers/Networking_manager_interface.hpp>

namespace jbatnozic {
namespace spempe {

using RPCReceiverContext = detail::RPCReceiverContext<GameContext, NetworkingManagerInterface>;

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_OTHER_RPC_RECEIVER_CONTEXT_HPP
