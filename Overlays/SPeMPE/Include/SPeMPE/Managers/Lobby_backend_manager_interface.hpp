#ifndef SPEMPE_MANAGERS_LOBBY_BACKEND_MANAGER_INTERFACE_HPP
#define SPEMPE_MANAGERS_LOBBY_BACKEND_MANAGER_INTERFACE_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Visitor.hpp>

#include <array>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include <SPeMPE/GameContext/Context_components.hpp>

namespace jbatnozic {
namespace spempe {

//! Information about an individual player in the lobby.
struct PlayerInfo {
    //! Name of the player (set by the player themselves).
    std::string name;

    //! Unique ID of this player (provided by their client).
    std::string uniqueId;

    //! IP address of the player in the form X.X.X.X (X = 0..255).
    std::string ipAddress;

    //! Some user-defined data that will be synced but not otherwise
    //! used by the lobby. This data is set by the clients.
    //! This custom data can be a simple string, a JSON string, a piece of
    //! base64-encoded binary data, a combination of those, or something
    //! completely different.
    //! Four separate slots are provided for this data.
    std::array<std::string, 4> customData;

    //! Returns whether the slot is empty (doesn't represent any
    //! connected player) or not.
    bool isEmpty() const;

    //! Returns whether the slot is complete. If the slot is empty, it is
    //! for sure also incomplete. However, a non-empty slot isn't necessarily
    //! complete (for example if a player connected but hasn't yet sent their
    //! name and unique ID).
    bool isComplete() const;

    //! Returns true if all members are equal (including the custom data).
    friend bool operator==(const PlayerInfo& aLhs, const PlayerInfo& aRhs);

    //! Opposite of operator==.
    friend bool operator!=(const PlayerInfo& aLhs, const PlayerInfo& aRhs);
};

struct LobbyBackendEvent; // Forward-declare

//! Represents a non-existing or undefined player.
constexpr int PLAYER_INDEX_UNKNOWN = -1;

class LobbyBackendManagerInterface : public ContextComponent {
public:
    ~LobbyBackendManagerInterface() override = default;

    enum class Mode {
        Uninitialized,
        Host,
        Client
    };

    virtual void setToHostMode(hobgoblin::PZInteger aLobbySize) = 0;

    virtual void setToClientMode(hobgoblin::PZInteger aLobbySize) = 0;

    virtual Mode getMode() const = 0;

    ///////////////////////////////////////////////////////////////////////////
    // HOST-MODE METHODS                                                     //
    ///////////////////////////////////////////////////////////////////////////
    // (note: use of these methods while in client mode will result in an exception being thrown)

    //! Use while in Host mode to map an index of a client connected to a SPeMPE
    //! Networking manager to their player index.
    //! As this refers to players in LockedIn slots, some players may not have been
    //! assigned a player index yet and in those cases this function will return
    //! PLAYER_INDEX_UNKNOWN.
    virtual int clientIdxToPlayerIdx(int aClientIdx) const = 0;

    //! Use while in Host mode to map a player index to their client index
    //! (client = client connected to a SPeMPE Networking manager).
    //! As this refers to players in LockedIn slots, it will return PLAYER_INDEX_UNKNOWN
    //! if the selected slot is empty.
    virtual int playerIdxToClientIdx(hobgoblin::PZInteger aPlayerIdx) const = 0;

    //! TODO
    virtual int clientIdxOfPlayerInPendingSlot(hobgoblin::PZInteger aSlotIndex) const = 0;

    //! TODO
    virtual void beginSwap(hobgoblin::PZInteger aSlotIndex1, hobgoblin::PZInteger aSlotIndex2) = 0;

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

    //! Returns true if a new event was written to 'aEvent' and false if the event queue was empty.
    virtual bool pollEvent(LobbyBackendEvent& aEvent) = 0;

    //! Returns the size of the lobby.
    virtual hobgoblin::PZInteger getSize() const = 0;

    //! Resizes the lobby (aNewLobbySize must be at least 1). When called on the Host size,
    //! it will set the lobby size for all connected clients. On client side, you can also
    //! call this but the lobby will keep reverting to the size set by the host.
    virtual void resize(hobgoblin::PZInteger aNewLobbySize) = 0;

    virtual const PlayerInfo& getLockedInPlayerInfo(hobgoblin::PZInteger aSlotIndex) const = 0;

    virtual const PlayerInfo& getPendingPlayerInfo(hobgoblin::PZInteger aSlotIndex) const = 0;

    //! @returns true if changes are pending for the specified slot; false otherwise.
    virtual bool areChangesPending(hobgoblin::PZInteger aSlotIndex) const = 0;

    //! @returns true if changes are pending for any slot; false otherwise.
    virtual bool areChangesPending() const = 0;
    
    //! Returns the player index (basically, the slot index in the lobby) of the local client.
    //! PLAYER_INDEX_UNKNOWN can be returned if the index wasn't yet assigned by the host.
    virtual int getLocalPlayerIndex() const = 0;

    //! Returns a string (in an undefined format) describing the entire state of the lobby.
    //! This function is meant for debugging purposes only.
    virtual std::string getEntireStateString() const = 0;

private:
    SPEMPE_CTXCOMP_TAG("jbatnozic::spempe::LobbyBackendManager");
};

///////////////////////////////////////////////////////////////////////////
// LOBBY BACKEND EVENT                                                   //
///////////////////////////////////////////////////////////////////////////

struct LobbyBackendEvent {
    struct LobbyLockedIn {
        bool somethingDidChange;
    };

    struct LobbyChanged {};

    using EventVariant = std::variant<
        LobbyChanged,
        LobbyLockedIn
    >;

    LobbyBackendEvent() = default;

    template <class taArgs>
    LobbyBackendEvent(taArgs&& aArgs)
        : eventVariant{std::forward<taArgs>(aArgs)}
    {
    }

    std::optional<EventVariant> eventVariant;

    template <class ...taCallables>
    void visit(taCallables&&... callables) {
        std::visit(
            hobgoblin::util::MakeVisitor([](const auto&) {}, std::forward<taCallables>(callables)...),
            eventVariant.value()
        );
    }

    template <class ...taCallables>
    void visit(taCallables&&... callables) const {
        std::visit(
            hobgoblin::util::MakeVisitor([](const auto&) {}, std::forward<taCallables>(callables)...),
            eventVariant.value()
        );
    }

    //! Unlike visit, a call to strictVisit will not compile unless a
    //! matching callable is provided for each event type.
    template <class ...taCallables>
    void strictVisit(taCallables&&... callables) {
        std::visit(hobgoblin::util::MakeVisitor(std::forward<taCallables>(callables)...), eventVariant.value());
    }

    //! Unlike visit, a call to strictVisit will not compile unless a
    //! matching callable is provided for each event type.
    template <class ...taCallables>
    void strictVisit(taCallables&&... callables) const {
        std::visit(hobgoblin::util::MakeVisitor(std::forward<taCallables>(callables)...), eventVariant.value());
    }
};

///////////////////////////////////////////////////////////////////////////
//INLINE IMPLEMENTATIONS                                                 //
///////////////////////////////////////////////////////////////////////////

inline
bool PlayerInfo::isEmpty() const {
    // It's guaranteed that at least the IP address of a connected
    // player is known, even if the name and unique ID aren't.
    return ipAddress.empty();
}

inline
bool PlayerInfo::isComplete() const {
    return !name.empty() && !ipAddress.empty() && !uniqueId.empty();
}

inline
bool PlayerInfo::isEmpty() const {
    // It's guaranteed that at least the IP address of a connected
    // player is known, even if the name and unique ID aren't.
    return ipAddress.empty();
}

inline
bool PlayerInfo::isComplete() const {
    return !name.empty() && !ipAddress.empty() && !uniqueId.empty();
}

inline
bool operator==(const PlayerInfo& aLhs, const PlayerInfo& aRhs) {
    return (aLhs.name == aRhs.name &&
            aLhs.ipAddress == aRhs.ipAddress &&
            aLhs.uniqueId == aRhs.uniqueId &&
            aLhs.customData == aRhs.customData);
}

inline
bool operator!=(const PlayerInfo& aLhs, const PlayerInfo& aRhs) {
    return !(aLhs == aRhs);
}

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_LOBBY_BACKEND_MANAGER_INTERFACE_HPP
