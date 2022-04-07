#ifndef SPEMPE_OTHER_SYNC_PARAMETERS_HPP
#define SPEMPE_OTHER_SYNC_PARAMETERS_HPP

#include <SPeMPE/GameContext/Game_context.hpp>
#include <SPeMPE/Managers/Networking_manager_interface.hpp>

namespace jbatnozic {
namespace spempe {

using SyncParameters = detail::SyncParameters<GameContext, NetworkingManagerInterface>;

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_OTHER_SYNC_PARAMETERS_HPP