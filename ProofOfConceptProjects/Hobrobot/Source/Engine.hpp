#ifndef HOBROBOT_ENGINE_HPP
#define HOBROBOT_ENGINE_HPP

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/QAO.hpp>
#include <Hobgoblin/RigelNet.hpp>
#include <SPeMPE/SPeMPE.hpp>

namespace hobrobot {

namespace spe = ::jbatnozic::spempe;
using namespace hg::qao; // All names from QAO are prefixed with QAO_
using namespace hg::rn;  // All names from RigelNet are prefixed with RN_

using MAuth         = spe::AuthorizationManagerInterface;
using MInput        = spe::InputSyncManagerInterface;
using MLobbyBackend = spe::LobbyBackendManagerInterface;
using MNetworking   = spe::NetworkingManagerInterface;
using MVarmap       = spe::SyncedVarmapManagerInterface;
using MWindow       = spe::WindowManagerInterface;

#define PRIORITY_VARMAPMGR      16
#define PRIORITY_NETWORKMGR     15
#define PRIORITY_LOBBYBACKMGR   14
#define PRIORITY_LOBBYFRONTMGR  13
#define PRIORITY_AUTHMGR        12
#define PRIORITY_GAMEPLAYMGR    10
#define PRIORITY_INPUTMGR        7
#define PRIORITY_PLAYERAVATAR    5
#define PRIORITY_ENVIRONMENTMGR  1
#define PRIORITY_WINDOWMGR       0

#define STATE_BUFFERING_LENGTH 2

} // namespace hobrobot

#endif // !HOBROBOT_ENGINE_HPP