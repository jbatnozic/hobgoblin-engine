
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

    void lockIn() override;

    hg::PZInteger clientIdxToPlayerIdx(hg::PZInteger aClientIdx) const override;
    hg::PZInteger playerIdxToClientIdx(hg::PZInteger aPlayerIdx) const override;

private:
    constexpr static int CLIENT_INDEX_NONE  = -2;
    constexpr static int CLIENT_INDEX_LOCAL = -1;

    struct TaggedPlayerId : PlayerId {
        int clientIndex = CLIENT_INDEX_NONE;

        bool operator==(const TaggedPlayerId& aOther) const {
            return
                name == aOther.name &&
                ipAddress == aOther.ipAddress &&
                port == aOther.port &&
                uniqueId == aOther.uniqueId;
        }

        bool operator!=(const TaggedPlayerId& aOther) const {
            return !(*this == aOther);
        }
    };

    std::vector<TaggedPlayerId> _lockedIn;
    std::vector<TaggedPlayerId> _desired;

    std::unordered_map<int, int> _clientIdxToPlayerIdxMapping;

    void _eventPreUpdate() override;
    void _eventDrawGUI() override;

    hg::PZInteger _getSize() const;
    bool _hasEntryForClient(const hobgoblin::RN_ConnectorInterface& aClient) const;
    hg::PZInteger _findOptimalPositionForClient(const hobgoblin::RN_ConnectorInterface& aClient) const;
    void _removeEntriesForDisconnectedPlayers(std::vector<TaggedPlayerId>& aTarget);
};

} // namespace spempe
} // namespace jbatnozic
