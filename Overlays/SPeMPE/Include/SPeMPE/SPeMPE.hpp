#ifndef SPEMPE_SPEMPE_HPP
#define SPEMPE_SPEMPE_HPP

// Game context

#include <SPeMPE/GameContext/Context_components.hpp>
#include <SPeMPE/GameContext/Game_context.hpp>
#include <SPeMPE/GameContext/Game_context_flag_validation.hpp>

// Game object framework

#include <SPeMPE/GameObjectFramework/Autodiff_state.hpp>
#include <SPeMPE/GameObjectFramework/Default_sync_impl.hpp>
#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>
#include <SPeMPE/GameObjectFramework/Sync_control_delegate.hpp>
#include <SPeMPE/GameObjectFramework/Sync_flags.hpp>
#include <SPeMPE/GameObjectFramework/Sync_id.hpp>
#include <SPeMPE/GameObjectFramework/Synchronized_object_registry.hpp>

// Managers

#include <SPeMPE/Managers/Authorization_manager_interface.hpp>
#include <SPeMPE/Managers/Authorization_manager_default.hpp>
#include <SPeMPE/Managers/Input_sync_manager_default.hpp>
#include <SPeMPE/Managers/Input_sync_manager_interface.hpp>
#include <SPeMPE/Managers/Lobby_backend_manager_default.hpp>
#include <SPeMPE/Managers/Lobby_backend_manager_interface.hpp>
#include <SPeMPE/Managers/Networking_manager_default.hpp>
#include <SPeMPE/Managers/Networking_manager_interface.hpp>
#include <SPeMPE/Managers/Synced_varmap_manager_default.hpp>
#include <SPeMPE/Managers/Synced_varmap_manager_interface.hpp>
#include <SPeMPE/Managers/Window_manager_default.hpp>
#include <SPeMPE/Managers/Window_manager_interface.hpp>

// Other

#include <SPeMPE/Utility/Rpc_receiver_context_template.hpp>
#include <SPeMPE/Utility/Rpc_receiver_context_user.hpp>
#include <SPeMPE/Utility/Window_frame_input_view.hpp>
#include <SPeMPE/Utility/Window_input_tracker.hpp>

#endif // !SPEMPE_SPEMPE_HPP
