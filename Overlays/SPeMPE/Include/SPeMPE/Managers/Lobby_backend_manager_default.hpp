// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef SPEMPE_MANAGERS_LOBBY_BACKEND_MANAGER_DEFAULT_HPP
#define SPEMPE_MANAGERS_LOBBY_BACKEND_MANAGER_DEFAULT_HPP

#include <SPeMPE/Managers/Lobby_backend_manager_interface.hpp>

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet.hpp>

#include <deque>
#include <unordered_map>
#include <vector>

#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>
#include <SPeMPE/Managers/Networking_manager_interface.hpp>

namespace jbatnozic {
namespace spempe {

/**
 * Needs: Networking manager, Synced varmap manager, [opt] Window manager
 */
class DefaultLobbyBackendManager
    : public LobbyBackendManagerInterface
    , public NonstateObject
    , private NetworkingEventListener
{
public:
    DefaultLobbyBackendManager(hg::QAO_RuntimeRef aRuntimeRef, int aExecutionPriority);

    ~DefaultLobbyBackendManager() override;

    void setToHostMode(hobgoblin::PZInteger aLobbySize) override;

    void setToClientMode(hobgoblin::PZInteger aLobbySize) override;

    Mode getMode() const override;

    ///////////////////////////////////////////////////////////////////////////
    // HOST-MODE METHODS                                                     //
    ///////////////////////////////////////////////////////////////////////////

    int clientIdxToPlayerIdx(int aClientIdx) const override;

    int playerIdxToClientIdx(hobgoblin::PZInteger aPlayerIdx) const override;

    int clientIdxOfPlayerInPendingSlot(hobgoblin::PZInteger aSlotIndex) const override;

    void beginSwap(hobgoblin::PZInteger aSlotIndex1, hobgoblin::PZInteger aSlotIndex2) override;

    bool lockInPendingChanges() override;

    bool resetPendingChanges() override;

    ///////////////////////////////////////////////////////////////////////////
    // CLIENT-MODE METHODS                                                   //
    ///////////////////////////////////////////////////////////////////////////
    
    void uploadLocalInfo() const override;

    ///////////////////////////////////////////////////////////////////////////
    // MODE-INDEPENDENT METHODS                                              //
    ///////////////////////////////////////////////////////////////////////////

    void setLocalName(const std::string& aName) override;

    const std::string& getLocalName() const override;

    void setLocalUniqueId(const std::string& aUniqueId) override;

    const std::string& getLocalUniqueId() const override;

    void setLocalCustomData(hobgoblin::PZInteger aIndex,
                            const std::string& aCustomData) override;

    const std::string& getLocalCustomData(hobgoblin::PZInteger aIndex) const override;

    bool pollEvent(LobbyBackendEvent& aEvent) override;

    hobgoblin::PZInteger getSize() const override;
    
    void resize(hobgoblin::PZInteger aNewLobbySize) override;

    const PlayerInfo& getLockedInPlayerInfo(hobgoblin::PZInteger aSlotIndex) const override;

    const PlayerInfo& getPendingPlayerInfo(hobgoblin::PZInteger aSlotIndex) const override;

    bool areChangesPending(hobgoblin::PZInteger aSlotIndex) const override;

    bool areChangesPending() const override;

    int getLocalPlayerIndex() const override;

    std::string getEntireStateString() const override;

private:
    Mode _mode = Mode::Uninitialized;

    struct ExtendedPlayerInfo : PlayerInfo {
        int clientIndex = CLIENT_INDEX_UNKNOWN;
        std::uint16_t port = 0;
        
        bool isSameAs(const hobgoblin::RN_ConnectorInterface& aClient) const;
    };
    friend bool operator==(const ExtendedPlayerInfo& aLhs, const ExtendedPlayerInfo& aRhs);
    friend bool operator!=(const ExtendedPlayerInfo& aLhs, const ExtendedPlayerInfo& aRhs);

    std::vector<ExtendedPlayerInfo> _lockedIn;
    std::vector<ExtendedPlayerInfo> _desired;

    std::unordered_map<int, int> _clientIdxToPlayerIdxMapping;

    PlayerInfo _localPlayerInfo;
    int _localPlayerIndex = PLAYER_INDEX_UNKNOWN;

    std::deque<LobbyBackendEvent> _eventQueue;

    void onNetworkingEvent(const hobgoblin::RN_Event& aEvent) override;

    void _eventBeginUpdate() override;
    void _eventPostUpdate() override;

    void _eventBeginUpdate_Host();
    void _eventBeginUpdate_Client();

    hg::PZInteger _getSize() const;
    void _scanForNewPlayers();
    bool _hasEntryForClient(const hobgoblin::RN_ConnectorInterface& aClient, int aClientIndex) const;
    hg::PZInteger _findOptimalPositionForClient(const hobgoblin::RN_ConnectorInterface& aClient) const;
    void _removeDesiredEntriesForDisconnectedPlayers();
    void _updateVarmapForLockedInEntry(hobgoblin::PZInteger aSlotIndex) const;
    void _updateVarmapForDesiredEntry(hobgoblin::PZInteger aSlotIndex) const;

    void _enqueueLobbyLockedIn(bool aSomethingDidChange);
    void _enqueueLobbyChanged();

    friend void USPEMPE_DefaultLobbyBackendManager_SetPlayerInfo_Impl(
        DefaultLobbyBackendManager& aLobbyMgr,
        const int aClientIndex,
        const std::string& aName,
        const std::string& aUniqueId,
        const std::string& aCustomData_0,
        const std::string& aCustomData_1,
        const std::string& aCustomData_2,
        const std::string& aCustomData_3
    );

    friend void USPEMPE_DefaultLobbyBackendManager_SetPlayerIndex_Impl(
        DefaultLobbyBackendManager& aLobbyMgr,
        hobgoblin::PZInteger aPlayerIndex
    );
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_LOBBY_BACKEND_MANAGER_DEFAULT_HPP

// clang-format on
