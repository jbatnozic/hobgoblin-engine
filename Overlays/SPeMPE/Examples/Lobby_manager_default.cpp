
#include "Lobby_manager_default.hpp"

#include <cassert>
#include <sstream>
#include <utility>

#include <Hobgoblin/Format.hpp>
#include <Hobgoblin/Logging.hpp>
#include <SPeMPE/Managers/Networking_manager_interface.hpp>
#include <SPeMPE/Managers/Synced_varmap_manager_interface.hpp>
#include <SPeMPE/Managers/Window_manager_interface.hpp>
#include <SFML/Graphics.hpp>

namespace jbatnozic {
namespace spempe {

using namespace hg::rn;

namespace {
constexpr auto LOG_ID = "::jbatnozic::spempe::DefaultLobbyManager";

bool IsConnected(const RN_ConnectorInterface& client) {
    return (client.getStatus() == RN_ConnectorStatus::Connected);
}

bool IsSameClient(const RN_ConnectorInterface& client, const PlayerId& player) {
    return (client.getRemoteInfo().ipAddress == player.ipAddress &&
            client.getRemoteInfo().port == player.port);
}

const std::string& MakeVarmapKey_LobbySize() {
    static std::string KEY = "::jbatnozic::spempe::DefaultLobbyManager_LOBBYSIZE";
    return KEY;
}

std::string MakeVarmapKey_LockedIn_Name(hg::PZInteger aSlotIndex) {
    return fmt::format("::jbatnozic::spempe::DefaultLobbyManager_LI_NAME_{}", aSlotIndex);
}

std::string MakeVarmapKey_LockedIn_IpAddr(hg::PZInteger aSlotIndex) {
    return fmt::format("::jbatnozic::spempe::DefaultLobbyManager_LI_IPADDR_{}", aSlotIndex);
}

std::string MakeVarmapKey_LockedIn_Port(hg::PZInteger aSlotIndex) {
    return fmt::format("::jbatnozic::spempe::DefaultLobbyManager_LI_PORT_{}", aSlotIndex);
}

std::string MakeVarmapKey_Desired_Name(hg::PZInteger aSlotIndex) {
    return fmt::format("::jbatnozic::spempe::DefaultLobbyManager_DE_NAME_{}", aSlotIndex);
}

std::string MakeVarmapKey_Desired_IpAddr(hg::PZInteger aSlotIndex) {
    return fmt::format("::jbatnozic::spempe::DefaultLobbyManager_DE_IPADDR_{}", aSlotIndex);
}

std::string MakeVarmapKey_Desired_Port(hg::PZInteger aSlotIndex) {
    return fmt::format("::jbatnozic::spempe::DefaultLobbyManager_DE_PORT_{}", aSlotIndex);
}

} // namespace

DefaultLobbyManager::DefaultLobbyManager(hg::QAO_RuntimeRef aRuntimeRef, int aExecutionPriority)
    : NonstateObject(aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "::jbatnozic::spempe::DefaultLobbyManager")
{    
}

DefaultLobbyManager::~DefaultLobbyManager() = default;

void DefaultLobbyManager::setToHostMode(hobgoblin::PZInteger aLobbySize) {
    _lockedIn.resize(hg::pztos(aLobbySize));
    _desired.resize(hg::pztos(aLobbySize));

    ccomp<SyncedVarmapManagerInterface>().setInt64(
        MakeVarmapKey_LobbySize(),
        aLobbySize
    );
}

void DefaultLobbyManager::setToClientMode(hobgoblin::PZInteger aLobbySize) {
    _lockedIn.resize(hg::pztos(aLobbySize));
    _desired.resize(hg::pztos(aLobbySize));
}

void DefaultLobbyManager::lockIn() {
    auto& varmap = ccomp<SyncedVarmapManagerInterface>();
    _lockedIn = _desired;
    for (hg::PZInteger i = 0; i < _getSize(); i += 1) {
        _clientIdxToPlayerIdxMapping[_lockedIn[i].clientIndex] = i;

        varmap.setString(MakeVarmapKey_LockedIn_Name(hg::stopz(i)), _lockedIn[i].name);
        varmap.setString(MakeVarmapKey_LockedIn_IpAddr(hg::stopz(i)), _lockedIn[i].ipAddress);
        varmap.setInt64(MakeVarmapKey_LockedIn_Port(hg::stopz(i)), _lockedIn[i].port);

        // send msg to client to update his Player ID
    }
}

hg::PZInteger DefaultLobbyManager::clientIdxToPlayerIdx(hg::PZInteger aClientIdx) const {
    const auto iter = _clientIdxToPlayerIdxMapping.find(aClientIdx);
    if (iter != _clientIdxToPlayerIdxMapping.end()) {
        return iter->second;
;    }
    // else...? TODO
}

hg::PZInteger DefaultLobbyManager::playerIdxToClientIdx(hg::PZInteger aPlayerIdx) const {
    return _lockedIn.at(hg::pztos(aPlayerIdx)).clientIndex;
}

// TODO: break up into smaller functions
void DefaultLobbyManager::_eventPreUpdate() {
    auto& netMgr = ccomp<NetworkingManagerInterface>();
    if (netMgr.isServer()) {
        auto& server = netMgr.getServer();
        
        //_removeEntriesForDisconnectedPlayers(_lockedIn);
        _removeEntriesForDisconnectedPlayers(_desired);

        // Check if all connected clients are represented in _desired. If not, add them.
        for (hg::PZInteger i = 0; i < server.getSize(); i += 1) {
            const auto& client = server.getClientConnector(i);

            if (IsConnected(client) && !_hasEntryForClient(client)) {
                const auto pos = _findOptimalPositionForClient(client);

                _desired[hg::pztos(pos)].name        = "<name>";
                _desired[hg::pztos(pos)].uniqueId    = "<uuid>";
                _desired[hg::pztos(pos)].ipAddress   = client.getRemoteInfo().ipAddress.toString();
                _desired[hg::pztos(pos)].port        = client.getRemoteInfo().port;
                _desired[hg::pztos(pos)].clientIndex = i;

                auto& varmap = ccomp<SyncedVarmapManagerInterface>();
                varmap.setString(MakeVarmapKey_Desired_Name(pos),   _desired[hg::pztos(pos)].name);
                varmap.setString(MakeVarmapKey_Desired_IpAddr(pos), _desired[hg::pztos(pos)].ipAddress);
                varmap.setInt64(MakeVarmapKey_Desired_Port(pos),    _desired[hg::pztos(pos)].port);

                HG_LOG_INFO(LOG_ID, "Inserted new player into slot {}", pos);
            }
        }

        if (ccomp<WindowManagerInterface>().getKeyboardInput().checkPressed(KbKey::L, KbInput::Mode::Direct)) {
            HG_LOG_INFO(LOG_ID, "Locking in slots");
            lockIn(); // TODO Temp.
        }
    } else {
        const auto& varmap = ccomp<SyncedVarmapManagerInterface>();

        // Check that size is correct and adjust if needed
        const auto& size = varmap.getInt64(MakeVarmapKey_LobbySize());
        if (size && *size != _getSize()) {
            _lockedIn.resize(*size);
            _desired.resize(*size);
        }

        // Load names & other info
        for (hg::PZInteger i = 0; i < _getSize(); i += 1) {
            {
                const auto n = varmap.getString(MakeVarmapKey_LockedIn_Name(i));
                _lockedIn[i].name = n ? *n : "";
            }
            {
                const auto a = varmap.getString(MakeVarmapKey_LockedIn_IpAddr(i));
                _lockedIn[i].ipAddress = a ? *a : "";
            }
            {
                const auto p = varmap.getInt64(MakeVarmapKey_LockedIn_Port(i));
                _lockedIn[i].port = static_cast<std::uint16_t>(p ? *p : 0);
            }
            {
                const auto n = varmap.getString(MakeVarmapKey_Desired_Name(i));
                _desired[i].name = n ? *n : "";
            }
            {
                const auto a = varmap.getString(MakeVarmapKey_Desired_IpAddr(i));
                _desired[i].ipAddress = a ? *a : "";
            }
            {
                const auto p = varmap.getInt64(MakeVarmapKey_Desired_Port(i));
                _desired[i].port = static_cast<std::uint16_t>(p ? *p : 0);
            }
        }
    }
}

void DefaultLobbyManager::_eventDrawGUI() {
    if (auto* winMgr = ctx().getComponentPtr<WindowManagerInterface>()) {
        std::stringstream ss;
        for (std::size_t i = 0; i < _lockedIn.size(); i += 1) {
            ss << "POSITION " << i << ":\n";
            ss << fmt::format("    {} {}:{}", _lockedIn[i].name, _lockedIn[i].ipAddress, _lockedIn[i].port);

            if (_lockedIn[i] != _desired[i]) {
                ss << fmt::format(" <- {} {}:{}\n", _desired[i].name, _desired[i].ipAddress, _desired[i].port);
            } else {
                ss << '\n';
            }
        }

        auto& canvas = winMgr->getCanvas();
        sf::Text text;
        text.setPosition(32.f, 32.f);
        text.setFont(hg::gr::BuiltInFonts::getFont(hg::gr::BuiltInFonts::TitilliumRegular));
        text.setFillColor(sf::Color::Yellow);
        text.setString(ss.str());
        canvas.draw(text);
    }
}

hg::PZInteger DefaultLobbyManager::_getSize() const {
    assert(_lockedIn.size() == _desired.size());
    return hg::stopz(_lockedIn.size());
}

bool DefaultLobbyManager::_hasEntryForClient(const RN_ConnectorInterface& aClient) const {
    for (const auto& entry : _desired) {
        if (IsSameClient(aClient, entry)) {
            return true;
        }
    }
    return false;
}

hg::PZInteger DefaultLobbyManager::_findOptimalPositionForClient(const RN_ConnectorInterface& aClient) const {
    struct {
        int pos = -1;
        int score = -1;
    } currentBest;

    for (std::size_t j = 0; j < _lockedIn.size(); j += 1) {
        if (_desired[j].clientIndex == CLIENT_INDEX_NONE) {
            // Apply scoring
            int score = 0;
            if (_lockedIn[j].ipAddress == aClient.getRemoteInfo().ipAddress) {
                score += 1;
            }
            // TODO score based on name?
            // TODO score based on ID?

            // Replace current best?
            if (score > currentBest.score) {
                currentBest.pos = static_cast<int>(j);
                currentBest.score = score;
            }
        }
    }

    if (currentBest.pos < 0) {
        throw hg::TracedRuntimeError("NO slot available for new client!");
    }

    return currentBest.pos;
}

void DefaultLobbyManager::_removeEntriesForDisconnectedPlayers(std::vector<TaggedPlayerId>& aTarget) {
    const auto& server = ccomp<NetworkingManagerInterface>().getServer();
    auto& varmap = ccomp<SyncedVarmapManagerInterface>();

    for (std::size_t i = 0; i < aTarget.size(); i += 1) {
        auto& player = aTarget[i];

        if (player.clientIndex == CLIENT_INDEX_NONE) {
            continue;
        }

        const auto& client = server.getClientConnector(player.clientIndex);
        if (!IsConnected(client) || !IsSameClient(client, player)) {
            HG_LOG_INFO(LOG_ID, "Removing disconnected client {}", player.clientIndex);

            player.name.clear();
            player.uniqueId.clear();
            player.ipAddress.clear();
            player.port = 0;
            player.clientIndex = CLIENT_INDEX_NONE;

            if (&aTarget == &_lockedIn) {
                varmap.setString(MakeVarmapKey_LockedIn_Name(hg::stopz(i)), "");
                varmap.setString(MakeVarmapKey_LockedIn_IpAddr(hg::stopz(i)), "");
                varmap.setInt64(MakeVarmapKey_LockedIn_Port(hg::stopz(i)), 0);
            }
            if (&aTarget == &_desired) {
                varmap.setString(MakeVarmapKey_Desired_Name(hg::stopz(i)), "");
                varmap.setString(MakeVarmapKey_Desired_IpAddr(hg::stopz(i)), "");
                varmap.setInt64(MakeVarmapKey_Desired_Port(hg::stopz(i)), 0);
            }
        }
    }
}

} // namespace spempe
} // namespace jbatnozic
