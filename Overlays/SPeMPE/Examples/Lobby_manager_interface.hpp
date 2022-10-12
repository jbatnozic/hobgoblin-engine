#ifndef SPEMPE_MANAGERS_LOBBY_MANAGER_INTERFACE_HPP
#define SPEMPE_MANAGERS_LOBBY_MANAGER_INTERFACE_HPP

#include <Hobgoblin/Common.hpp>

#include <cstdint>

#include <SPeMPE/GameContext/Context_components.hpp>

namespace jbatnozic {
namespace spempe {

struct PlayerId {
    std::string name      = "";
    std::string uniqueId  = "";
    std::string ipAddress = "";
    std::uint16_t port    = 0;
};

class LobbyManagerInterface : public ContextComponent {
public:
    ~LobbyManagerInterface() override = default;

    virtual void setToHostMode(hobgoblin::PZInteger aLobbySize) = 0;

    virtual void setToClientMode(hobgoblin::PZInteger aLobbySize) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // ???                                                                   //
    ///////////////////////////////////////////////////////////////////////////
    
    virtual void lockIn() = 0;

    virtual hobgoblin::PZInteger clientIdxToPlayerIdx(hobgoblin::PZInteger aClientIdx) const = 0;
    virtual hobgoblin::PZInteger playerIdxToClientIdx(hobgoblin::PZInteger aPlayerIdx) const = 0;

private:
    SPEMPE_CTXCOMP_TAG("jbatnozic::spempe::LobbyManager");
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_LOBBY_MANAGER_INTERFACE_HPP