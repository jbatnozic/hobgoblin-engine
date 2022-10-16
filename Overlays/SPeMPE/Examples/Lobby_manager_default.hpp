
#pragma once

#include "Lobby_manager_interface.hpp"

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet.hpp>

#include <unordered_map>
#include <vector>

#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>

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

    ///////////////////////////////////////////////////////////////////////////
    // ???                                                                   //
    ///////////////////////////////////////////////////////////////////////////

    void setLocalName(const std::string& aNewName) override;

    std::string getLocalName() const override;

    void setLocalUniqueId(const std::string& aNewUniqueId) override;

    std::string getLocalUniqueId() const override;

    void uploadLocalInfo() const override;

    hobgoblin::PZInteger getSize() const override { return 0; }

    void resize(hobgoblin::PZInteger aNewLobbySize) const override {}

    void lockIn() override;

    int getLocalPlayerIndex() const override;

    hg::PZInteger clientIdxToPlayerIdx(hg::PZInteger aClientIdx) const override;
    hg::PZInteger playerIdxToClientIdx(hg::PZInteger aPlayerIdx) const override;

private:
    constexpr static int CLIENT_INDEX_NONE  = -2;
    constexpr static int CLIENT_INDEX_LOCAL = -1;

    struct ExtendedPlayerInfo : PlayerInfo {
        int clientIndex = CLIENT_INDEX_NONE;
        std::uint16_t port = 0;

        // TODO Turn into regular method
        bool operator==(const ExtendedPlayerInfo& aOther) const {
            return
                name == aOther.name &&
                ipAddress == aOther.ipAddress &&
                port == aOther.port &&
                uniqueId == aOther.uniqueId;
        }

        bool operator!=(const ExtendedPlayerInfo& aOther) const {
            return !(*this == aOther);
        }
    };

    std::vector<ExtendedPlayerInfo> _lockedIn;
    std::vector<ExtendedPlayerInfo> _desired;

    std::unordered_map<int, int> _clientIdxToPlayerIdxMapping;

    PlayerInfo _localPlayerInfo;
    int _localPlayerIndex = PLAYER_INDEX_UNKNOWN;

    void _eventPreUpdate() override;
    void _eventDrawGUI() override;

    hg::PZInteger _getSize() const;
    bool _hasEntryForClient(const hobgoblin::RN_ConnectorInterface& aClient) const;
    hg::PZInteger _findOptimalPositionForClient(const hobgoblin::RN_ConnectorInterface& aClient) const;
    void _removeEntriesForDisconnectedPlayers(std::vector<ExtendedPlayerInfo>& aTarget);

    friend void USPEMPE_DefaultLobbyManager_SetPlayerInfo_Impl(
        DefaultLobbyManager& aLobbyMgr,
        const std::string& aName,
        const std::string& aUniqueId,
        int aClientIndex
    );
};

} // namespace spempe
} // namespace jbatnozic
