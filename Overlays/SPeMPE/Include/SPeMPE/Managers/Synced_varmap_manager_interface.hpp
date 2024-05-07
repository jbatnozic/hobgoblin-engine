// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef SPEMPE_MANAGERS_SYNCED_VARMAP_MANAGER_INTERFACE_HPP
#define SPEMPE_MANAGERS_SYNCED_VARMAP_MANAGER_INTERFACE_HPP

#include <SPeMPE/GameContext/Context_components.hpp>

#include <cstdint>
#include <optional>
#include <string>

namespace jbatnozic {
namespace spempe {

class SyncedVarmapManagerInterface : public ContextComponent {
public:
    virtual ~SyncedVarmapManagerInterface() override = default;

    enum class Mode {
        Uninitialized,
        Host,
        Client
    };

    virtual void setToMode(Mode aMode) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // VALUE GETTERS                                                         //
    ///////////////////////////////////////////////////////////////////////////

    virtual auto getInt64(const std::string& aKey) const -> std::optional<std::int64_t> = 0;
    virtual auto getDouble(const std::string& aKey) const -> std::optional<double> = 0;
    virtual auto getString(const std::string& aKey) const -> std::optional<std::string> = 0;

    ///////////////////////////////////////////////////////////////////////////
    // VALUE SETTERS (HOST SIDE)                                             //
    ///////////////////////////////////////////////////////////////////////////

    virtual void setInt64(const std::string& aKey, std::int64_t aValue) = 0;
    virtual void setDouble(const std::string& aKey, double aValue) = 0;
    virtual void setString(const std::string& aKey, const std::string& aValue) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // VALUE SET REQUESTERS (CLIENT SIDE)                                    //
    ///////////////////////////////////////////////////////////////////////////

    virtual void requestToSetInt64(const std::string& aKey, std::int64_t aValue) = 0;
    virtual void requestToSetDouble(const std::string& aKey, double aValue) = 0;
    virtual void requestToSetString(const std::string& aKey, const std::string& aValue) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // WRITE PERMISSION SETTERS (HOST SIDE)                                  //
    ///////////////////////////////////////////////////////////////////////////

    constexpr static bool ALLOWED   = true;
    constexpr static bool FORBIDDEN = false;

    virtual void int64SetClientWritePermission(const std::string& aKey, 
                                               hg::PZInteger aPlayerIndex, 
                                               bool aAllowed) = 0;

    virtual void doubleSetClientWritePermission(const std::string& aKey,
                                                hg::PZInteger aPlayerIndex,
                                                bool aAllowed) = 0;

    virtual void stringSetClientWritePermission(const std::string& aKey,
                                                hg::PZInteger aPlayerIndex,
                                                bool aAllowed) = 0;
    // TODO

private:
    SPEMPE_CTXCOMP_TAG("jbatnozic::spempe::SyncedVarmapManagerInterface");
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_SYNCED_VARMAP_MANAGER_INTERFACE_HPP

// clang-format on
