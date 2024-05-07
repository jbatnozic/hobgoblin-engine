// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef SPEMPE_MANAGERS_SYNCED_VARMAP_MANAGER_DEFAULT_HPP
#define SPEMPE_MANAGERS_SYNCED_VARMAP_MANAGER_DEFAULT_HPP

#include <Hobgoblin/Utility/Dynamic_bitset.hpp>
#include <Hobgoblin/Utility/Packet.hpp>
#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>
#include <SPeMPE/Managers/Networking_manager_interface.hpp>
#include <SPeMPE/Managers/Synced_varmap_manager_interface.hpp>

#include <unordered_map>

namespace jbatnozic {
namespace spempe {

/**
 * Needs: Networking manager
 */
class DefaultSyncedVarmapManager 
    : public SyncedVarmapManagerInterface
    , public NonstateObject
    , private NetworkingEventListener {
public:
    DefaultSyncedVarmapManager(hg::QAO_RuntimeRef aRuntimeRef, int aExecutionPriority);

    ~DefaultSyncedVarmapManager() override;

    void setToMode(Mode aMode) override;

    void onNetworkingEvent(const hg::RN_Event& aEvent) override;

    ///////////////////////////////////////////////////////////////////////////
    // VALUE GETTERS                                                         //
    ///////////////////////////////////////////////////////////////////////////

    auto getInt64(const std::string& aKey) const -> std::optional<std::int64_t> override;
    auto getDouble(const std::string& aKey) const -> std::optional<double> override;
    auto getString(const std::string& aKey) const -> std::optional<std::string> override;

    ///////////////////////////////////////////////////////////////////////////
    // VALUE SETTERS (HOST SIDE)                                             //
    ///////////////////////////////////////////////////////////////////////////

    void setInt64(const std::string& aKey, std::int64_t aValue) override;
    void setDouble(const std::string& aKey, double aValue) override;
    void setString(const std::string& aKey, const std::string& aValue) override;

    ///////////////////////////////////////////////////////////////////////////
    // VALUE SET REQUESTERS (CLIENT SIDE)                                    //
    ///////////////////////////////////////////////////////////////////////////

    void requestToSetInt64(const std::string& aKey, std::int64_t aValue) override;
    void requestToSetDouble(const std::string& aKey, double aValue) override;
    void requestToSetString(const std::string& aKey, const std::string& aValue) override;

    ///////////////////////////////////////////////////////////////////////////
    // WRITE PERMISSION SETTERS (HOST SIDE)                                  //
    ///////////////////////////////////////////////////////////////////////////

    void int64SetClientWritePermission(const std::string& aKey,
                                       hg::PZInteger aPlayerIndex,
                                       bool aAllowed) override;

    void doubleSetClientWritePermission(const std::string& aKey,
                                        hg::PZInteger aPlayerIndex,
                                        bool aAllowed) override;

    void stringSetClientWritePermission(const std::string& aKey,
                                        hg::PZInteger aPlayerIndex,
                                        bool aAllowed) override;

private:
    friend void USPEMPE_DefaultSyncedVarmapManager_SetValues(DefaultSyncedVarmapManager&,
                                                             hobgoblin::util::Packet&);

    friend void USPEMPE_DefaultSyncedVarmapManager_SetValueRequested(DefaultSyncedVarmapManager&,
                                                                     hobgoblin::PZInteger aPlayerIndex,
                                                                     hobgoblin::util::Packet&);

    template <class taVal>
    struct Elem {
        std::optional<taVal> value = {};
        hobgoblin::util::DynamicBitset permissions = {};
    };

    NetworkingManagerInterface& _netMgr;

    Mode _mode = Mode::Uninitialized;

    std::unordered_map<std::string, Elem<std::int64_t>> _int64Values;
    std::unordered_map<std::string, Elem<double>>       _doubleValues;
    std::unordered_map<std::string, Elem<std::string>>  _stringValues;

    hobgoblin::util::Packet _stateUpdates;

    void _eventEndUpdate() override;

    static void _packValue(const std::string& aKey, 
                           std::int64_t aValue, 
                           hobgoblin::util::Packet& aPacket);

    static void _packValue(const std::string& aKey, 
                           double aValue, 
                           hobgoblin::util::Packet& aPacket);

    static void _packValue(const std::string& aKey, 
                           const std::string& aValue,
                           hobgoblin::util::Packet& aPacket);

    void _unpackValues(hobgoblin::util::Packet& aPacket);

    bool _setValueRequested(hobgoblin::PZInteger aPlayerIndex, hobgoblin::util::Packet& aPacket);

    void _sendFullState(hg::PZInteger aClientIndex) const;
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_SYNCED_VARMAP_MANAGER_DEFAULT_HPP

// clang-format on
