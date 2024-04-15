// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef SPEMPE_MANAGERS_AUTHORIZATION_MANAGER_INTERFACE_HPP
#define SPEMPE_MANAGERS_AUTHORIZATION_MANAGER_INTERFACE_HPP

#include <SPeMPE/GameContext/Context_components.hpp>
#include <SPeMPE/Managers/Authorization_manager_interface.hpp>

#include <optional>
#include <string>

namespace jbatnozic {
namespace spempe {

// Possible strategies:
//  - first connected client (default)
//  - client from lobby by unique ID
//  - by slot
//  - ?

using AuthToken = std::string;

class AuthorizationManagerInterface : public ContextComponent {
public:
    ~AuthorizationManagerInterface() override = default;

    enum class Mode {
        Uninitialized,
        Host,
        Client
    };

    virtual void setToHostMode(/* TODO: provide auth strategy*/) = 0;

    virtual void setToClientMode() = 0;

    virtual Mode getMode() const = 0;

    virtual std::optional<AuthToken> getLocalAuthToken() = 0;

private:
    SPEMPE_CTXCOMP_TAG("jbatnozic::spempe::AuthorizationManagerInterface");
};


} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_AUTHORIZATION_MANAGER_INTERFACE_HPP

// clang-format on
