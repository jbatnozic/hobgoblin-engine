#ifndef SPEMPE_MANAGERS_LOBBY_MANAGER_INTERFACE_HPP
#define SPEMPE_MANAGERS_LOBBY_MANAGER_INTERFACE_HPP

#include <Hobgoblin/Common.hpp>

#include <array>
#include <cstdint>
#include <string>

#include <SPeMPE/GameContext/Context_components.hpp>

namespace jbatnozic {
namespace spempe {

//! TODO
struct PlayerInfo {
    //! Name of the player (set by the player themselves).
    std::string name;

    //! Unique ID of this player (provided by their client).
    std::string uniqueId;

    //! IP address of the player in the form of X.X.X.X (X = 0..255).
    std::string ipAddress;

    //! Some user-defined data that will be synced but not otherwise
    //! by the lobby. This data is set by the clients.
    //! This custom data can be a simple string, a JSON string, a piece of
    //! base64-encoded binary data, a combination of those, or something
    //! completely different.
    //! Four separate slots are provided for this data.
    std::array<std::string, 4> customData;
};


//! TODO
constexpr int PLAYER_INDEX_UNKNOWN = -1;

class LobbyManagerInterface : public ContextComponent {
public:
    ~LobbyManagerInterface() override = default;

    virtual void setToHostMode(hobgoblin::PZInteger aLobbySize) = 0;

    virtual void setToClientMode(hobgoblin::PZInteger aLobbySize) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // HOST-MODE METHODS                                                     //
    ///////////////////////////////////////////////////////////////////////////
    // (note: use of these methods while in client mode will result in an exception being thrown)

    //! Use while in Host mode to map an index of a client connected to a SPeMPE
    //! Networking manager to their player index.
    virtual hobgoblin::PZInteger clientIdxToPlayerIdx(hobgoblin::PZInteger aClientIdx) const = 0;

    //! Use while in Host mode to map a player index to their client index
    //! (client = client connected to a SPeMPE Networking manager).
    virtual hobgoblin::PZInteger playerIdxToClientIdx(hobgoblin::PZInteger aPlayerIdx) const = 0;

    //! TODO
    //! @returns true if any locked-in slot was changed by this; false otherwise.
    virtual bool lockInPendingChanges() = 0;

    //! TODO
    //! @returns true if any pending slot was changed by this; false otherwise.
    virtual bool resetPendingChanges() = 0;

    ///////////////////////////////////////////////////////////////////////////
    // CLIENT-MODE METHODS                                                   //
    ///////////////////////////////////////////////////////////////////////////
    // (note: use of these methods while in host mode will result in an exception being thrown)

    //! Call while in Client mode to send local info (i.e. name, unique ID, and custom
    //! data) to the server. It's recommended to call this as soon as connection to the
    //! server is established (but after all the set* methods obviously). Should you
    //! change any of the local data using the set* methods, you need to call this again.
    virtual void uploadLocalInfo() const = 0;

    ///////////////////////////////////////////////////////////////////////////
    // MODE-INDEPENDENT METHODS                                              //
    ///////////////////////////////////////////////////////////////////////////

    virtual void setLocalName(const std::string& aName) = 0;

    virtual const std::string& getLocalName() const = 0;

    virtual void setLocalUniqueId(const std::string& aUniqueId) = 0;

    virtual const std::string& getLocalUniqueId() const = 0;

    virtual void setLocalCustomData(hobgoblin::PZInteger aIndex,
                                    const std::string& aCustomData) = 0;

    virtual const std::string& getLocalCustomData(hobgoblin::PZInteger aIndex) const = 0;

    //! Returns the size of the lobby.
    virtual hobgoblin::PZInteger getSize() const = 0;

    //! Resizes the lobby (aNewLobbySize must be at least 1). When called on the Host size,
    //! it will set the lobby size for all connected clients. On client side, you can also
    //! call this but the lobby will keep reverting to the size set by the host.
    virtual void resize(hobgoblin::PZInteger aNewLobbySize) const = 0;

    virtual const PlayerInfo& getLockedInPlayerInfo(hobgoblin::PZInteger aSlotIndex) const = 0;

    virtual const PlayerInfo& getPendingPlayerInfo(hobgoblin::PZInteger aSlotIndex) const = 0;

    //! @returns true if changes are pending for the specified slot; false otherwise.
    virtual bool areChangesPending(hobgoblin::PZInteger aSlotIndex) const = 0;

    //! @returns true if changes are pending for any slot; false otherwise.
    virtual bool areChangesPending() const = 0;
    
    //! Returns the player index (basically, the slot index in the lobby) of the local client.
    //! PLAYER_INDEX_UNKNOWN can be returned if the index wasn't yet assigned by the host.
    virtual int getLocalPlayerIndex() const = 0;

private:
    SPEMPE_CTXCOMP_TAG("jbatnozic::spempe::LobbyManager");
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_LOBBY_MANAGER_INTERFACE_HPP