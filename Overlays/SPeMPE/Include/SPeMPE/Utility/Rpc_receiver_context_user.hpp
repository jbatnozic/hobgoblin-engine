#ifndef SPEMPE_OTHER_RPC_RECEIVER_CONTEXT_USER_HPP
#define SPEMPE_OTHER_RPC_RECEIVER_CONTEXT_USER_HPP

#include <SPeMPE/Managers/Networking_manager_interface.hpp>
#include <SPeMPE/Utility/Rpc_receiver_context_template.hpp>

namespace jbatnozic {
namespace spempe {

using RPCReceiverContext = RPCReceiverContextTemplate<NetworkingManagerInterface>;

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_OTHER_RPC_RECEIVER_CONTEXT_USER_HPP
