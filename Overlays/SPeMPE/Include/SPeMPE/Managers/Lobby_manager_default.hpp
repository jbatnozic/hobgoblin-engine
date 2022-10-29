#ifndef SPEMPE_MANAGERS_LOBBY_MANAGER_DEFAULT_HPP
#define SPEMPE_MANAGERS_LOBBY_MANAGER_DEFAULT_HPP

#include <SPeMPE/Managers/Lobby_manager_interface.hpp>

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet.hpp>

#include <unordered_map>
#include <vector>

#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>
#include <SPeMPE/Managers/Networking_manager_interface.hpp>

namespace jbatnozic {
namespace spempe {

/**
 * Needs: Networking manager, Synced varmap manager, [opt] Window manager
 */
class DefaultLobbyManager
    : public LobbyManagerInterface
    , public NonstateObject
{
public:
    DefaultLobbyManager(hg::QAO_RuntimeRef aRuntimeRef, int aExecutionPriority);

    ~DefaultLobbyManager() override;

    void setToHostMode(hobgoblin::PZInteger aLobbySize) override;

    void setToClientMode(hobgoblin::PZInteger aLobbySize) override;

    Mode getMode() const override;

    ///////////////////////////////////////////////////////////////////////////
    // HOST-MODE METHODS                                                     //
    ///////////////////////////////////////////////////////////////////////////

    hobgoblin::PZInteger clientIdxToPlayerIdx(int aClientIdx) const override;

    int playerIdxToClientIdx(hobgoblin::PZInteger aPlayerIdx) const override;

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

    hobgoblin::PZInteger getSize() const override;

    void resize(hobgoblin::PZInteger aNewLobbySize) override;

    const PlayerInfo& getLockedInPlayerInfo(hobgoblin::PZInteger aSlotIndex) const override;

    const PlayerInfo& getPendingPlayerInfo(hobgoblin::PZInteger aSlotIndex) const override;

    bool areChangesPending(hobgoblin::PZInteger aSlotIndex) const override;

    bool areChangesPending() const override;

    int getLocalPlayerIndex() const override;

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

    void _eventPreUpdate() override;
    void _eventDrawGUI() override;

    hg::PZInteger _getSize() const;
    bool _hasEntryForClient(const hobgoblin::RN_ConnectorInterface& aClient, int aClientIndex) const;
    hg::PZInteger _findOptimalPositionForClient(const hobgoblin::RN_ConnectorInterface& aClient) const;
    void _removeDesiredEntriesForDisconnectedPlayers();
    void _updateVarmapForLockedInEntry(hobgoblin::PZInteger aSlotIndex) const;
    void _updateVarmapForDesiredEntry(hobgoblin::PZInteger aSlotIndex) const;

    friend void USPEMPE_DefaultLobbyManager_SetPlayerInfo_Impl(
        DefaultLobbyManager& aLobbyMgr,
        const int aClientIndex,
        const std::string& aName,
        const std::string& aUniqueId,
        const std::string& aCustomData_0,
        const std::string& aCustomData_1,
        const std::string& aCustomData_2,
        const std::string& aCustomData_3
    );

    friend void USPEMPE_DefaultLobbyManager_SetPlayerIndex_Impl(
        DefaultLobbyManager& aLobbyMgr,
        hobgoblin::PZInteger aPlayerIndex
    );
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_LOBBY_MANAGER_DEFAULT_HPP