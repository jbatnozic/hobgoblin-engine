
#include <SPeMPE/Managers/Lobby_backend_manager_default.hpp>

#include <algorithm>
#include <cassert>
#include <sstream>
#include <utility>

#include <Hobgoblin/Format.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/RigelNet_macros.hpp>
#include <SPeMPE/Managers/Synced_varmap_manager_interface.hpp>
#include <SPeMPE/Managers/Window_manager_interface.hpp>
#include <SPeMPE/Managers/Networking_manager_interface.hpp>
#include <SPeMPE/Utility/Rpc_receiver_context_template.hpp>
#include <SFML/Graphics.hpp>

namespace jbatnozic {
namespace spempe {

namespace hg = hobgoblin;
using namespace hg::rn;

namespace {
constexpr auto LOG_ID = "jbatnozic::spempe::DefaultLobbyBackendManager";

constexpr auto CUSTOM_DATA_LEN = hg::stopz(std::tuple_size_v<decltype(PlayerInfo::customData)>);

bool IsConnected(const RN_ConnectorInterface& client) {
    return (client.getStatus() == RN_ConnectorStatus::Connected);
}

const std::string& MakeVarmapKey_LobbySize() {
    static std::string KEY = "jbatnozic::spempe::DefaultLobbyBackendManager_LOBBYSIZE";
    return KEY;
}

std::string MakeVarmapKey_LockedIn_Name(hg::PZInteger aSlotIndex) {
    return fmt::format("jbatnozic::spempe::DefaultLobbyBackendManager_LI_NAME_{}", aSlotIndex);
}

std::string MakeVarmapKey_LockedIn_UniqueId(hg::PZInteger aSlotIndex) {
    return fmt::format("jbatnozic::spempe::DefaultLobbyBackendManager_LI_UUID_{}", aSlotIndex);
}

std::string MakeVarmapKey_LockedIn_IpAddr(hg::PZInteger aSlotIndex) {
    return fmt::format("jbatnozic::spempe::DefaultLobbyBackendManager_LI_IPADDR_{}", aSlotIndex);
}

std::string MakeVarmapKey_LockedIn_CData(hg::PZInteger aSlotIndex, hg::PZInteger aCDataIndex) {
    return fmt::format("jbatnozic::spempe::DefaultLobbyBackendManager_LI_CDAT{}_{}", aCDataIndex, aSlotIndex);
}

std::string MakeVarmapKey_Desired_Name(hg::PZInteger aSlotIndex) {
    return fmt::format("jbatnozic::spempe::DefaultLobbyBackendManager_DE_NAME_{}", aSlotIndex);
}

std::string MakeVarmapKey_Desired_UniqueId(hg::PZInteger aSlotIndex) {
    return fmt::format("jbatnozic::spempe::DefaultLobbyBackendManager_DE_UUID_{}", aSlotIndex);
}

std::string MakeVarmapKey_Desired_IpAddr(hg::PZInteger aSlotIndex) {
    return fmt::format("jbatnozic::spempe::DefaultLobbyBackendManager_DE_IPADDR_{}", aSlotIndex);
}

std::string MakeVarmapKey_Desired_CData(hg::PZInteger aSlotIndex, hg::PZInteger aCDataIndex) {
    return fmt::format("jbatnozic::spempe::DefaultLobbyBackendManager_DE_CDAT{}_{}", aCDataIndex, aSlotIndex);
}

} // namespace

void USPEMPE_DefaultLobbyBackendManager_SetPlayerInfo_Impl(
    DefaultLobbyBackendManager& aLobbyMgr,
    const int aClientIndex,
    const std::string& aName,
    const std::string& aUniqueId,
    const std::string& aCustomData_0,
    const std::string& aCustomData_1,
    const std::string& aCustomData_2,
    const std::string& aCustomData_3
) {
    auto& self = aLobbyMgr;

    hg::PZInteger l;
    for (l = 0; l < self._getSize(); l += 1) {
        auto& entry = self._lockedIn[hg::pztos(l)];
        if (entry.clientIndex == aClientIndex) {
            entry.name = aName;
            entry.uniqueId = aUniqueId;
            entry.customData[0] = aCustomData_0;
            entry.customData[1] = aCustomData_1;
            entry.customData[2] = aCustomData_2;
            entry.customData[3] = aCustomData_3;
            break;
        }
    }
    if (l != self._getSize()) {
        self._updateVarmapForLockedInEntry(l);
    }
    else {
        HG_LOG_WARN(LOG_ID, 
                    "USPEMPE_DefaultLobbyBackendManager_SetPlayerInfo_Impl - "
                    "Could not find client with corresponding idx {} amongst locked in clients.",
                    aClientIndex);
    }

    hg::PZInteger d;
    for (d = 0; d < self._getSize(); d += 1) {
        auto& entry = self._desired[hg::pztos(d)];
        if (entry.clientIndex == aClientIndex) {
            entry.name = aName;
            entry.uniqueId = aUniqueId;
            entry.customData[0] = aCustomData_0;
            entry.customData[1] = aCustomData_1;
            entry.customData[2] = aCustomData_2;
            entry.customData[3] = aCustomData_3;
            break;
        }
    }
    if (d != self._getSize()) {
        self._updateVarmapForDesiredEntry(d);
    }
    else {
        HG_LOG_WARN(LOG_ID, 
                    "USPEMPE_DefaultLobbyBackendManager_SetPlayerInfo_Impl - "
                    "Could not find client with corresponding idx {} amongst pending clients.",
                    aClientIndex);
    }
}

RN_DEFINE_RPC(USPEMPE_DefaultLobbyBackendManager_SetPlayerInfo,
              RN_ARGS(std::string&, aName, 
                      std::string&, aUniqueId,
                      std::string&, aCustomData_0,
                      std::string&, aCustomData_1,
                      std::string&, aCustomData_2,
                      std::string&, aCustomData_3)) {
    RN_NODE_IN_HANDLER().callIfServer(
        [&](RN_ServerInterface& aServer) {
            const auto rc = SPEMPE_GET_RPC_RECEIVER_CONTEXT(aServer);
            USPEMPE_DefaultLobbyBackendManager_SetPlayerInfo_Impl(
                dynamic_cast<DefaultLobbyBackendManager&>(
                    rc.gameContext.getComponent<LobbyBackendManagerInterface>()
                ),
                rc.senderIndex,
                aName,
                aUniqueId,
                aCustomData_0,
                aCustomData_1,
                aCustomData_2,
                aCustomData_3
            );
        });
    RN_NODE_IN_HANDLER().callIfClient(
        [](RN_ClientInterface&) {
            throw RN_IllegalMessage();
        });
}

void USPEMPE_DefaultLobbyBackendManager_SetPlayerIndex_Impl(
    DefaultLobbyBackendManager& aLobbyMgr,
    hobgoblin::PZInteger aPlayerIndex
) {
    auto& self = aLobbyMgr;
    if (self._localPlayerIndex != aPlayerIndex) {
        self._localPlayerIndex = aPlayerIndex;
        HG_LOG_INFO(LOG_ID, "Local player index set to {}.", aPlayerIndex);
    }
}

RN_DEFINE_RPC(USPEMPE_DefaultLobbyBackendManager_SetPlayerIndex,
              RN_ARGS(hg::PZInteger, aPlayerIndex)) {
    RN_NODE_IN_HANDLER().callIfServer(
        [](RN_ServerInterface&) {
            throw RN_IllegalMessage();
        });
    RN_NODE_IN_HANDLER().callIfClient(
        [=](RN_ClientInterface& aClient) {
            const auto rc = SPEMPE_GET_RPC_RECEIVER_CONTEXT(aClient);
            USPEMPE_DefaultLobbyBackendManager_SetPlayerIndex_Impl(
                dynamic_cast<DefaultLobbyBackendManager&>(rc.gameContext.getComponent<LobbyBackendManagerInterface>()),
                aPlayerIndex
            );
        });
}

bool operator==(const DefaultLobbyBackendManager::ExtendedPlayerInfo& aLhs,
                const DefaultLobbyBackendManager::ExtendedPlayerInfo& aRhs) {
    return (static_cast<const PlayerInfo&>(aLhs) == static_cast<const PlayerInfo&>(aRhs) &&
            aLhs.clientIndex == aRhs.clientIndex &&
            aLhs.port == aRhs.port);
}

bool operator!=(const DefaultLobbyBackendManager::ExtendedPlayerInfo& aLhs,
                const DefaultLobbyBackendManager::ExtendedPlayerInfo& aRhs) {
    return !(aLhs == aRhs);
}


bool DefaultLobbyBackendManager::ExtendedPlayerInfo::isSameAs(const hg::RN_ConnectorInterface& aClient) const {
    const auto ip_ = aClient.getRemoteInfo().ipAddress.toString();
    const auto port_ = aClient.getRemoteInfo().port;
    return (ip_ == ipAddress && port_ == port);
}

DefaultLobbyBackendManager::DefaultLobbyBackendManager(hg::QAO_RuntimeRef aRuntimeRef, int aExecutionPriority)
    : NonstateObject(aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "::jbatnozic::spempe::DefaultLobbyBackendManager")
{    
}

DefaultLobbyBackendManager::~DefaultLobbyBackendManager() = default;

void DefaultLobbyBackendManager::setToHostMode(hobgoblin::PZInteger aLobbySize) {
    SPEMPE_VALIDATE_GAME_CONTEXT_FLAGS(ctx(), privileged==true, networking==true);

    _mode = Mode::Host;
    
    resize(aLobbySize);

    ccomp<SyncedVarmapManagerInterface>().setInt64(
        MakeVarmapKey_LobbySize(),
        aLobbySize
    );

    // Set local player
    _lockedIn[0].name = "<< SPeMPE SERVER >>";
    _lockedIn[0].uniqueId = "n/a";
    _lockedIn[0].ipAddress = "127.0.0.1";
    _lockedIn[0].port = 0;
    _lockedIn[0].clientIndex = CLIENT_INDEX_LOCAL;

    _desired[0] = _lockedIn[0];

    _localPlayerIndex = 0;
    _clientIdxToPlayerIdxMapping[CLIENT_INDEX_LOCAL] = 0;

    _updateVarmapForLockedInEntry(0);
    _updateVarmapForDesiredEntry(0);
}

void DefaultLobbyBackendManager::setToClientMode(hobgoblin::PZInteger aLobbySize) {
    SPEMPE_VALIDATE_GAME_CONTEXT_FLAGS(ctx(), privileged==false, networking==true);

    _mode = Mode::Client;
    resize(aLobbySize);
}

DefaultLobbyBackendManager::Mode DefaultLobbyBackendManager::getMode() const {
    return _mode;
}

///////////////////////////////////////////////////////////////////////////
// HOST-MODE METHODS                                                     //
///////////////////////////////////////////////////////////////////////////

int DefaultLobbyBackendManager::clientIdxToPlayerIdx(int aClientIdx) const {
    if (_mode != Mode::Host) {
        HG_THROW_TRACED(hg::TracedLogicError, 0,
                        "This method can only be called while in Host mode!");
    }

    const auto iter = _clientIdxToPlayerIdxMapping.find(aClientIdx);
    if (iter != _clientIdxToPlayerIdxMapping.end()) {
        return iter->second;
    }
    
    return PLAYER_INDEX_UNKNOWN;
}

int DefaultLobbyBackendManager::playerIdxToClientIdx(hg::PZInteger aPlayerIdx) const {
    if (_mode != Mode::Host) {
        HG_THROW_TRACED(hg::TracedLogicError, 0,
                        "This method can only be called while in Host mode!");
    }

    return _lockedIn.at(hg::pztos(aPlayerIdx)).clientIndex;
}

int DefaultLobbyBackendManager::clientIdxOfPlayerInPendingSlot(hobgoblin::PZInteger aSlotIndex) const {
    if (_mode != Mode::Host) {
        HG_THROW_TRACED(hg::TracedLogicError, 0,
                        "This method can only be called while in Host mode!");
    }

    return _desired.at(hg::pztos(aSlotIndex)).clientIndex;
}

void DefaultLobbyBackendManager::beginSwap(hobgoblin::PZInteger aSlotIndex1, hobgoblin::PZInteger aSlotIndex2) {
    if (_mode != Mode::Host) {
        HG_THROW_TRACED(hg::TracedLogicError, 0,
                        "This method can only be called while in Host mode!");
    }
    HG_VALIDATE_ARGUMENT(aSlotIndex1 < _getSize(), "aSlotIndex1 ({}) out of bounds.", aSlotIndex1);
    HG_VALIDATE_ARGUMENT(aSlotIndex2 < _getSize(), "aSlotIndex2 ({}) out of bounds.", aSlotIndex2);

    if (aSlotIndex1 == aSlotIndex2) {
        return;
    }

    std::swap(_desired[hg::pztos(aSlotIndex1)], _desired[hg::pztos(aSlotIndex2)]);

    _updateVarmapForDesiredEntry(aSlotIndex1);
    _updateVarmapForDesiredEntry(aSlotIndex2);

    _enqueueLobbyChanged();
}

bool DefaultLobbyBackendManager::lockInPendingChanges() {
    if (_mode != Mode::Host) {
        HG_THROW_TRACED(hg::TracedLogicError, 0,
                        "This method can only be called while in Host mode!");
    }

    if (_lockedIn == _desired) {
        _enqueueLobbyLockedIn(false);
        HG_LOG_INFO(LOG_ID, "Slots locked in (no change).");
        return false; // Nothing to change
    }

    _lockedIn = _desired;
    for (hg::PZInteger slotIndex = 0; slotIndex < _getSize(); slotIndex += 1) {
        const auto clientIndex = _lockedIn[slotIndex].clientIndex;

        _clientIdxToPlayerIdxMapping[clientIndex] = slotIndex;

        _updateVarmapForLockedInEntry(slotIndex);

        auto& netMgr = ccomp<NetworkingManagerInterface>();
        if (clientIndex >= 0) {
            Compose_USPEMPE_DefaultLobbyBackendManager_SetPlayerIndex(
                netMgr.getNode(),
                _lockedIn[slotIndex].clientIndex,
                slotIndex
            );
        } else if (clientIndex == CLIENT_INDEX_LOCAL) {
            _localPlayerIndex = slotIndex;
        }
    }

    _enqueueLobbyChanged();
    _enqueueLobbyLockedIn(true);
    
    HG_LOG_INFO(LOG_ID, "Slots locked in.");
    return true;
}

bool DefaultLobbyBackendManager::resetPendingChanges() {
    if (_mode != Mode::Host) {
        HG_THROW_TRACED(hg::TracedLogicError, 0,
                        "This method can only be called while in Host mode!");
    }

    HG_LOG_WARN(LOG_ID, "Skipping lobby reset because it is not yet implemented.");

    //bool somethingDidChange = false;
    //for (hg::PZInteger i = 0; i < _getSize(); i += 1) {
    //    if (areChangesPending(i)) {
    //        somethingDidChange = true;
    //        _desired[hg::pztos(i)] = _lockedIn[hg::pztos(i)];
    //    }
    //}
    //if (somethingDidChange) {
    //    _eventBeginUpdate(); // TODO
    //}
    //return somethingDidChange;

    return false;
}

///////////////////////////////////////////////////////////////////////////
// CLIENT-MODE METHODS                                                   //
///////////////////////////////////////////////////////////////////////////

void DefaultLobbyBackendManager::uploadLocalInfo() const {
    if (_mode != Mode::Client) {
        HG_THROW_TRACED(hg::TracedLogicError, 0,
                        "This method can only be called while in Client mode!");
    }

    auto& netMgr = ccomp<NetworkingManagerInterface>();
    Compose_USPEMPE_DefaultLobbyBackendManager_SetPlayerInfo(
        netMgr.getNode(),
        RN_COMPOSE_FOR_ALL,
        _localPlayerInfo.name,
        _localPlayerInfo.uniqueId,
        _localPlayerInfo.customData[0],
        _localPlayerInfo.customData[1],
        _localPlayerInfo.customData[2],
        _localPlayerInfo.customData[3]
    );
}

///////////////////////////////////////////////////////////////////////////
// MODE-INDEPENDENT METHODS                                              //
///////////////////////////////////////////////////////////////////////////

void DefaultLobbyBackendManager::setLocalName(const std::string& aName) {
    _localPlayerInfo.name = aName;

    if (_mode == Mode::Host) {
        auto& varmap = ccomp<SyncedVarmapManagerInterface>();
        {
            const auto localSlot =
                std::find_if(_lockedIn.begin(), _lockedIn.end(),
                             [](const ExtendedPlayerInfo& aEpi) { return aEpi.clientIndex == CLIENT_INDEX_LOCAL; }
            ) - _lockedIn.begin();
            _lockedIn[static_cast<std::size_t>(localSlot)].name = aName;
            varmap.setString(MakeVarmapKey_LockedIn_Name(static_cast<hg::PZInteger>(localSlot)), aName);
        }
        {
            const auto localSlot =
                std::find_if(_desired.begin(), _desired.end(),
                             [](const ExtendedPlayerInfo& aEpi) { return aEpi.clientIndex == CLIENT_INDEX_LOCAL; }
            ) - _desired.begin();
            _desired[static_cast<std::size_t>(localSlot)].name = aName;
            varmap.setString(MakeVarmapKey_Desired_Name(static_cast<hg::PZInteger>(localSlot)), aName);
        }
    }
}

const std::string& DefaultLobbyBackendManager::getLocalName() const {
    return _localPlayerInfo.name;
}

void DefaultLobbyBackendManager::setLocalUniqueId(const std::string& aUniqueId) {
    _localPlayerInfo.uniqueId = aUniqueId;

    if (_mode == Mode::Host) {
        auto& varmap = ccomp<SyncedVarmapManagerInterface>();
        {
            const auto localSlot =
                std::find_if(_lockedIn.begin(), _lockedIn.end(),
                             [](const ExtendedPlayerInfo& aEpi) { return aEpi.clientIndex == CLIENT_INDEX_LOCAL; }
            ) - _lockedIn.begin();
            _lockedIn[static_cast<std::size_t>(localSlot)].uniqueId = aUniqueId;
            varmap.setString(MakeVarmapKey_LockedIn_UniqueId(static_cast<hg::PZInteger>(localSlot)), aUniqueId);
        }
        {
            const auto localSlot =
                std::find_if(_desired.begin(), _desired.end(),
                             [](const ExtendedPlayerInfo& aEpi) { return aEpi.clientIndex == CLIENT_INDEX_LOCAL; }
            ) - _desired.begin();
            _desired[static_cast<std::size_t>(localSlot)].uniqueId = aUniqueId;
            varmap.setString(MakeVarmapKey_Desired_UniqueId(static_cast<hg::PZInteger>(localSlot)), aUniqueId);
        }
    }
}

const std::string& DefaultLobbyBackendManager::getLocalUniqueId() const {
    return _localPlayerInfo.uniqueId;
}

void DefaultLobbyBackendManager::setLocalCustomData(hobgoblin::PZInteger aIndex,
                                             const std::string& aCustomData) {
    _localPlayerInfo.customData.at(hg::pztos(aIndex)) = aCustomData;

    if (_mode == Mode::Host) {
        auto& varmap = ccomp<SyncedVarmapManagerInterface>();
        {
            const auto localSlot =
                std::find_if(_lockedIn.begin(), _lockedIn.end(),
                             [](const ExtendedPlayerInfo& aEpi) { return aEpi.clientIndex == CLIENT_INDEX_LOCAL; }
            ) - _lockedIn.begin();
            _lockedIn[static_cast<std::size_t>(localSlot)].customData[hg::pztos(aIndex)] = aCustomData;
            varmap.setString(MakeVarmapKey_LockedIn_CData(static_cast<hg::PZInteger>(localSlot), aIndex), aCustomData);
        }
        {
            const auto localSlot =
                std::find_if(_desired.begin(), _desired.end(),
                             [](const ExtendedPlayerInfo& aEpi) { return aEpi.clientIndex == CLIENT_INDEX_LOCAL; }
            ) - _desired.begin();
            _desired[static_cast<std::size_t>(localSlot)].customData[hg::pztos(aIndex)] = aCustomData;
            varmap.setString(MakeVarmapKey_Desired_CData(static_cast<hg::PZInteger>(localSlot), aIndex), aCustomData);
        }
    }
}

const std::string& DefaultLobbyBackendManager::getLocalCustomData(hobgoblin::PZInteger aIndex) const {
    return _localPlayerInfo.customData.at(hg::pztos(aIndex));
}

bool DefaultLobbyBackendManager::pollEvent(LobbyBackendEvent& aEvent) {
    if (_eventQueue.empty()) {
        return false;
    }
    aEvent = std::move(_eventQueue.front());
    _eventQueue.pop_front();
    return true;
}

hg::PZInteger DefaultLobbyBackendManager::getSize() const {
    return _getSize();
}

void DefaultLobbyBackendManager::resize(hobgoblin::PZInteger aNewLobbySize) {
    HG_VALIDATE_ARGUMENT(aNewLobbySize >= 1, "aNewLobbySize must be at least 1.");

    _lockedIn.resize(hg::pztos(aNewLobbySize));
    _desired.resize(hg::pztos(aNewLobbySize));

    if (_mode == Mode::Host) {
        ccomp<SyncedVarmapManagerInterface>().setInt64(
            MakeVarmapKey_LobbySize(),
            aNewLobbySize
        );
    }
}

const PlayerInfo& DefaultLobbyBackendManager::getLockedInPlayerInfo(hobgoblin::PZInteger aSlotIndex) const {
    return _lockedIn.at(hg::pztos(aSlotIndex));
}

const PlayerInfo& DefaultLobbyBackendManager::getPendingPlayerInfo(hobgoblin::PZInteger aSlotIndex) const {
    return _desired.at(hg::pztos(aSlotIndex));
}

bool  DefaultLobbyBackendManager::areChangesPending(hobgoblin::PZInteger aSlotIndex) const {
    return (_lockedIn.at(hg::pztos(aSlotIndex)) != _desired[hg::pztos(aSlotIndex)]);
}

bool  DefaultLobbyBackendManager::areChangesPending() const {
    for (hg::PZInteger i = 0; i < _getSize(); i += 1) {
        if (areChangesPending(i)) {
            return true;
        }
    }
    return false;
}

int DefaultLobbyBackendManager::getLocalPlayerIndex() const {
    return _localPlayerIndex;
}

std::string DefaultLobbyBackendManager::getEntireStateString() const {
    std::stringstream ss;
    for (std::size_t i = 0; i < _lockedIn.size(); i += 1) {
        ss << "SLOT " << i << ":\n";
        ss << fmt::format("    {} {}:{}", _lockedIn[i].name, _lockedIn[i].ipAddress, _lockedIn[i].port);

        if (static_cast<const PlayerInfo&>(_lockedIn[i]) != static_cast<const PlayerInfo&>(_desired[i])) {
            ss << fmt::format(" <- {} {}:{}\n", _desired[i].name, _desired[i].ipAddress, _desired[i].port);
        }
        else {
            ss << '\n';
        }
    }
    return ss.str();
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS                                                       //
///////////////////////////////////////////////////////////////////////////

void DefaultLobbyBackendManager::_eventBeginUpdate() {
    switch (_mode) {
        case Mode::Host:
            _eventBeginUpdate_Host();
            break;

        case Mode::Client:
            _eventBeginUpdate_Client();
            break;

        default: {}
    }
}

void DefaultLobbyBackendManager::_eventPostUpdate() {
    if (!_eventQueue.empty()) {
        HG_LOG_WARN(LOG_ID, "Clearing events that weren't polled.");
        _eventQueue.clear();
    }
}

void DefaultLobbyBackendManager::_eventBeginUpdate_Host() {
    auto& netMgr = ccomp<NetworkingManagerInterface>();
    auto& server = netMgr.getServer();

    _removeDesiredEntriesForDisconnectedPlayers();

    // Check if all connected clients are represented in _desired. If not, add them.
    for (hg::PZInteger i = 0; i < server.getSize(); i += 1) {
        const auto& client = server.getClientConnector(i);

        if (IsConnected(client) && !_hasEntryForClient(client, i)) {
            const auto pos = _findOptimalPositionForClient(client);

            _desired[hg::pztos(pos)].name        = "";
            _desired[hg::pztos(pos)].uniqueId    = "";
            _desired[hg::pztos(pos)].ipAddress   = client.getRemoteInfo().ipAddress.toString();
            _desired[hg::pztos(pos)].port        = client.getRemoteInfo().port;
            _desired[hg::pztos(pos)].clientIndex = i;

            _updateVarmapForDesiredEntry(pos);

            _enqueueLobbyChanged();
            HG_LOG_INFO(LOG_ID, "Inserted new player into slot {}", pos);
        }
    }
}

void DefaultLobbyBackendManager::_eventBeginUpdate_Client() {
    const auto& varmap = ccomp<SyncedVarmapManagerInterface>();

    // Check that size is correct and adjust if needed
    const auto& size = varmap.getInt64(MakeVarmapKey_LobbySize());
    if (size && *size != _getSize()) {
        resize(hg::stopz(*size));
    }

    // Load names & other info
    bool lobbyDidChange = false;

    #define LOAD_VALUE(_slot_, _field_, _key_) \
        do { \
            auto newval_ = varmap.getString(_key_); \
            if (newval_.has_value() && (_slot_._field_) != *newval_) { \
                (_slot_._field_) = std::move(*newval_); \
                lobbyDidChange = true; \
            } \
        } while (false)

    #define LOAD_VALUE_NO_CHANGE_DETECTION(_slot_, _field_, _key_) \
        do { \
            auto newval_ = varmap.getString(_key_); \
            if (newval_.has_value() && (_slot_._field_) != *newval_) { \
                (_slot_._field_) = std::move(*newval_); \
            } \
        } while (false)

    for (hg::PZInteger i = 0; i < _getSize(); i += 1) {
        // Locked in slots:
        LOAD_VALUE(_lockedIn[i], name,      MakeVarmapKey_LockedIn_Name(i));
        LOAD_VALUE(_lockedIn[i], ipAddress, MakeVarmapKey_LockedIn_IpAddr(i));
        LOAD_VALUE(_lockedIn[i], uniqueId,  MakeVarmapKey_LockedIn_UniqueId(i));

        LOAD_VALUE_NO_CHANGE_DETECTION(_lockedIn[i], customData[0], MakeVarmapKey_LockedIn_CData(i, 0));
        LOAD_VALUE_NO_CHANGE_DETECTION(_lockedIn[i], customData[1], MakeVarmapKey_LockedIn_CData(i, 1));
        LOAD_VALUE_NO_CHANGE_DETECTION(_lockedIn[i], customData[2], MakeVarmapKey_LockedIn_CData(i, 2));
        LOAD_VALUE_NO_CHANGE_DETECTION(_lockedIn[i], customData[3], MakeVarmapKey_LockedIn_CData(i, 3));

        // Desired slots:
        LOAD_VALUE(_desired[i], name,      MakeVarmapKey_Desired_Name(i));
        LOAD_VALUE(_desired[i], ipAddress, MakeVarmapKey_Desired_IpAddr(i));
        LOAD_VALUE(_desired[i], uniqueId,  MakeVarmapKey_Desired_UniqueId(i));

        LOAD_VALUE_NO_CHANGE_DETECTION(_desired[i], customData[0], MakeVarmapKey_Desired_CData(i, 0));
        LOAD_VALUE_NO_CHANGE_DETECTION(_desired[i], customData[1], MakeVarmapKey_Desired_CData(i, 1));
        LOAD_VALUE_NO_CHANGE_DETECTION(_desired[i], customData[2], MakeVarmapKey_Desired_CData(i, 2));
        LOAD_VALUE_NO_CHANGE_DETECTION(_desired[i], customData[3], MakeVarmapKey_Desired_CData(i, 3));
    }

    #undef LOAD_VALUE_NO_CHANGE_DETECTION
    #undef LOAD_VALUE

    if (lobbyDidChange) {
        _enqueueLobbyChanged();
    }
}

hg::PZInteger DefaultLobbyBackendManager::_getSize() const {
    assert(_lockedIn.size() == _desired.size());
    return hg::stopz(_lockedIn.size());
}

bool DefaultLobbyBackendManager::_hasEntryForClient(const RN_ConnectorInterface& aClient, int aClientIndex) const {
    for (const auto& entry : _desired) {
        if (entry.isSameAs(aClient) && entry.clientIndex == aClientIndex) {
            return true;
        }
    }
    return false;
}

hg::PZInteger DefaultLobbyBackendManager::_findOptimalPositionForClient(const RN_ConnectorInterface& aClient) const {
    struct {
        int pos = -1;
        int score = -1;
    } currentBest;

    for (std::size_t j = 0; j < _lockedIn.size(); j += 1) {
        if (_desired[j].clientIndex == CLIENT_INDEX_UNKNOWN) {
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
        HG_THROW_TRACED(hg::TracedRuntimeError, 0, "No slot available for new client!");
    }

    return currentBest.pos;
}

void DefaultLobbyBackendManager::_removeDesiredEntriesForDisconnectedPlayers() {
    const auto& server = ccomp<NetworkingManagerInterface>().getServer();

    for (std::size_t i = 0; i < hg::pztos(_getSize()); i += 1) {
        auto& player = _desired[i];

        if (player.clientIndex == CLIENT_INDEX_UNKNOWN || player.clientIndex == CLIENT_INDEX_LOCAL) {
            continue;
        }

        const auto& client = server.getClientConnector(player.clientIndex);
        if (!IsConnected(client) || !player.isSameAs(client)) {
            HG_LOG_INFO(LOG_ID, "Removing disconnected client {}", player.clientIndex);

            player.name.clear();
            player.uniqueId.clear();
            player.ipAddress.clear();
            player.port = 0;
            player.clientIndex = CLIENT_INDEX_UNKNOWN;

            _updateVarmapForDesiredEntry(i);
            // TODO ev: lobby changed
        }
    }
}

void DefaultLobbyBackendManager::_updateVarmapForLockedInEntry(hobgoblin::PZInteger aSlotIndex) const {
    const auto& entry = _lockedIn[hg::pztos(aSlotIndex)];
    auto& varmap = ccomp<SyncedVarmapManagerInterface>();

    varmap.setString(MakeVarmapKey_LockedIn_Name(aSlotIndex), entry.name);
    varmap.setString(MakeVarmapKey_LockedIn_IpAddr(aSlotIndex), entry.ipAddress);
    varmap.setString(MakeVarmapKey_LockedIn_UniqueId(aSlotIndex), entry.uniqueId);

    for (hg::PZInteger i = 0; i < CUSTOM_DATA_LEN; i += 1) {
        varmap.setString(MakeVarmapKey_LockedIn_CData(aSlotIndex, i), entry.customData[i]);
    }
}

void DefaultLobbyBackendManager::_updateVarmapForDesiredEntry(hobgoblin::PZInteger aSlotIndex) const {
    const auto& entry = _desired[hg::pztos(aSlotIndex)];
    auto& varmap = ccomp<SyncedVarmapManagerInterface>();

    varmap.setString(MakeVarmapKey_Desired_Name(aSlotIndex), entry.name);
    varmap.setString(MakeVarmapKey_Desired_IpAddr(aSlotIndex), entry.ipAddress);
    varmap.setString(MakeVarmapKey_Desired_UniqueId(aSlotIndex), entry.uniqueId);

    for (hg::PZInteger i = 0; i < CUSTOM_DATA_LEN; i += 1) {
        varmap.setString(MakeVarmapKey_Desired_CData(aSlotIndex, i), entry.customData[i]);
    }
}

void DefaultLobbyBackendManager::_enqueueLobbyLockedIn(bool aSomethingDidChange) {
    LobbyBackendEvent ev;
    ev.eventVariant = LobbyBackendEvent::LobbyLockedIn{aSomethingDidChange};
    _eventQueue.push_back(ev);
}

void DefaultLobbyBackendManager::_enqueueLobbyChanged() {
    LobbyBackendEvent ev;
    ev.eventVariant = LobbyBackendEvent::LobbyChanged{};
    _eventQueue.push_back(ev);
}

} // namespace spempe
} // namespace jbatnozic
