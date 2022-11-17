
#include <SPeMPE/Managers/Authorization_manager_default.hpp>

#include <SPeMPE/Managers/Lobby_manager_interface.hpp>
#include <SPeMPE/Managers/Networking_manager_interface.hpp>

#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/Utility/Hash.hpp>
#include <Hobgoblin/Utility/Randomization.hpp>

#include <functional>
#include <unordered_set>

#include <Hobgoblin/RigelNet_macros.hpp>

namespace jbatnozic {
namespace spempe {

namespace hg = hobgoblin;
using namespace hg::rn;

namespace {

constexpr auto LOG_ID = "jbatnozic::spempe::DefaultAuthorizationManager";

std::string GenerateRandomString(hg::PZInteger aStringLength) {
    return hg::util::DoWith32bitRNG(
        [=](std::mt19937& aRNG) {
            std::uniform_int_distribution<char> dist{33, 126};
            std::string result;
            result.resize(hg::pztos(aStringLength), ' ');
            for (hg::PZInteger i = 0; i < aStringLength; i += 1) {
                result[hg::pztos(i)] = dist(aRNG);
            }
            return result;
        });
}

} // namespace
} // namespace spempe
} // namespace jbatnozic

namespace std {
#define PInfo jbatnozic::spempe::detail::PlayerInfoWithIndex
template <>
struct hash<PInfo> {
    size_t operator()(const PInfo& aInfo) const {
        using jbatnozic::hobgoblin::util::CalcHashAndCombine;
        size_t result = 0;
        result = CalcHashAndCombine(aInfo.name,        result);
        result = CalcHashAndCombine(aInfo.uniqueId,    result);
        result = CalcHashAndCombine(aInfo.ipAddress,   result);
        result = CalcHashAndCombine(aInfo.clientIndex, result);
        return result;
    }
};
#undef PInfo
} // namespace std

namespace jbatnozic {
namespace spempe {
namespace {

using PlayerInfoWISet = std::unordered_set<detail::PlayerInfoWithIndex>;

PlayerInfoWISet ScanLobbyManagerForAllCurrentlyConnectedPlayers(LobbyManagerInterface& aLobbyMgr) {
    PlayerInfoWISet result;
    for (hg::PZInteger i = 0; i < aLobbyMgr.getSize(); i += 1) {
        const auto& playerInfo = aLobbyMgr.getPendingPlayerInfo(i);
        if (playerInfo.isComplete()) {
            result.insert({
                playerInfo.name,
                playerInfo.uniqueId,
                playerInfo.ipAddress,
                aLobbyMgr.clientIdxOfPlayerInPendingSlot(i)
            });
        }
    }
    return result;
}
} // namespace

void USPEMPE_DefaultAuthorizationManager_SetLocalAuthToken(
    DefaultAuthorizationManager& aAuthMgr,
    const AuthToken& aToken
) {
    aAuthMgr._localAuthToken = aToken;
    HG_LOG_INFO(LOG_ID, "Local node authorized by the host.");
}

RN_DEFINE_RPC(SetLocalAuthToken, RN_ARGS(AuthToken&, aToken)) {
    RN_NODE_IN_HANDLER().callIfClient(
        [&](RN_ClientInterface& aClient) {
            const auto sp = SPEMPE_GET_SYNC_PARAMS(aClient);
            auto& authMgr = dynamic_cast<DefaultAuthorizationManager&>(
                sp.context.getComponent<AuthorizationManagerInterface>()
            );
            USPEMPE_DefaultAuthorizationManager_SetLocalAuthToken(authMgr, aToken);
        });
    RN_NODE_IN_HANDLER().callIfServer(
        [&](RN_ServerInterface&) {
            throw RN_IllegalMessage();
        });
}

#define AUTH_TOKEN_LENGTH 50

DefaultAuthorizationManager::DefaultAuthorizationManager(hg::QAO_RuntimeRef aRuntimeRef,
                                                         int aExecutionPriority)
    : NonstateObject(aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "jbatnozic::spempe::DefaultAuthorizationManager")
{
}

DefaultAuthorizationManager::~DefaultAuthorizationManager() = default;

void DefaultAuthorizationManager::setToHostMode(/* TODO: provide auth strategy*/) {
    _mode = Mode::Host;
    _localAuthToken = "<placeholder-token>";
}


void DefaultAuthorizationManager::setToClientMode() {
    _mode = Mode::Client; // TODO
}

DefaultAuthorizationManager::Mode DefaultAuthorizationManager::getMode() const {
    return _mode;
}

std::optional<AuthToken> DefaultAuthorizationManager::getLocalAuthToken() {
    return _localAuthToken;
}

void DefaultAuthorizationManager::_eventPreUpdate() {
    // TODO - Temporary implementation
    if (_mode != Mode::Host) {
        return;
    }

    auto& aLobbyMgr = ccomp<LobbyManagerInterface>();
    const auto players = ScanLobbyManagerForAllCurrentlyConnectedPlayers(aLobbyMgr);

    if (_hasCurrentlyAuthorizedPlayer()) {
        if (players.find(_currentAuthorizedPlayer) == players.end()) {
            // Authorized player disconnected; authorize another one if able
            for (const auto& player : players) {
                // We never want to authorize the local player (they always
                // have full privileges to do anything they want)
                if (player.clientIndex != CLIENT_INDEX_LOCAL) {
                    _localAuthToken = GenerateRandomString(AUTH_TOKEN_LENGTH);
                    _authorizePlayer(
                        player,
                        ccomp<NetworkingManagerInterface>(),
                        ccomp<SyncedVarmapManagerInterface>()
                    );
                }
                break;
            }
        }
    }
    else {
        if (!players.empty()) {
            _localAuthToken = GenerateRandomString(AUTH_TOKEN_LENGTH);
            for (const auto& player : players) {
                // We never want to authorize the local player (they always
                // have full privileges to do anything they want)
                if (player.clientIndex != CLIENT_INDEX_LOCAL) {
                    _localAuthToken = GenerateRandomString(AUTH_TOKEN_LENGTH);
                    _authorizePlayer(
                        player,
                        ccomp<NetworkingManagerInterface>(),
                        ccomp<SyncedVarmapManagerInterface>()
                    );
                    break;
                }
            }
        }
    }
}

bool DefaultAuthorizationManager::_hasCurrentlyAuthorizedPlayer() const {
    return _currentAuthorizedPlayer.clientIndex != CLIENT_INDEX_UNKNOWN;
}

void DefaultAuthorizationManager::_authorizePlayer(
    const detail::PlayerInfoWithIndex& aPlayerToAuthorize,
    NetworkingManagerInterface& aNetMgr,
    SyncedVarmapManagerInterface& aSvmMgr
) {
    _currentAuthorizedPlayer = aPlayerToAuthorize;
    Compose_SetLocalAuthToken(aNetMgr.getNode(), aPlayerToAuthorize.clientIndex, *_localAuthToken);
    // set varmap values...
    HG_LOG_INFO(LOG_ID, "Authorized player {} ({}).", aPlayerToAuthorize.name, aPlayerToAuthorize.ipAddress);
}

} // namespace spempe
} // namespace jbatnozic
