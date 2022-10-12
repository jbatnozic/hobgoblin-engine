#ifndef SPEMPE_MANAGERS_NETWORKING_MANAGER_INTERFACE_HPP
#define SPEMPE_MANAGERS_NETWORKING_MANAGER_INTERFACE_HPP

#include <Hobgoblin/RigelNet.hpp>

#include <SPeMPE/GameContext/Context_components.hpp>
#include <SPeMPE/GameObjectFramework/Synchronized_object_registry.hpp>

namespace jbatnozic {
namespace spempe {

namespace hg = ::jbatnozic::hobgoblin;

class NetworkingEventListener {
public:
    virtual void onNetworkingEvent(const hg::RN_Event& ev) = 0;
};

constexpr int PLAYER_INDEX_NONE         = -2; //! Not everything has been initialized yet.
constexpr int PLAYER_INDEX_UNKNOWN      = -1; //! Client index not yet received from Server.
constexpr int PLAYER_INDEX_LOCAL_PLAYER =  0; //! Local player = not connected through RigelNet.

class NetworkingManagerInterface : public ContextComponent {
public:
    virtual ~NetworkingManagerInterface() = default;

    using NodeType   = hg::RN_NodeInterface;
    using ServerType = hg::RN_ServerInterface;
    using ClientType = hg::RN_ClientInterface;

    ///////////////////////////////////////////////////////////////////////////
    // CONFIGURATION                                                         //
    ///////////////////////////////////////////////////////////////////////////

    enum class Mode {
        Uninitialized, //! Underlying node is a Dummy (fulfills the interface but does nothing)
        Server,        //! Underlying node is a Server
        Client,        //! Underlying node is a Client
    };

    virtual void setToMode(Mode aMode) = 0;

    virtual Mode getMode() const = 0;

    virtual bool isUninitialized() const = 0;
    virtual bool isServer() const = 0;
    virtual bool isClient() const = 0;

    ///////////////////////////////////////////////////////////////////////////
    // NODE ACCESS                                                           //
    ///////////////////////////////////////////////////////////////////////////

    virtual NodeType& getNode() const = 0;

    virtual ServerType& getServer() const = 0;

    virtual ClientType& getClient() const = 0;

    ///////////////////////////////////////////////////////////////////////////
    // LISTENER MANAGEMENT                                                   //
    ///////////////////////////////////////////////////////////////////////////

    virtual void addEventListener(NetworkingEventListener& listener) = 0;

    virtual void removeEventListener(NetworkingEventListener& listener) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // SYNCHRONIZATION                                                       //
    ///////////////////////////////////////////////////////////////////////////

    virtual RegistryId getRegistryId() = 0;

    virtual hg::PZInteger getStateBufferingLength() const = 0;

    //! buffering = 0: no delay, everything displayed immediately
    //! buffering > 0: everything delayed by a number of steps to give time to
    //!                account for and amortize latency, reduce jittering etc.
    //! WARNING: This only sets the value locally! If you want to have the same 
    //! buffering lengths for all players, you need to solve that problem manually
    //! (there are no hard blockers to everyone having different buffering
    //! lengths, everything will still work).
    virtual void setStateBufferingLength(hg::PZInteger aNewStateBufferingLength) = 0;

    //! Every N frames the server sends a pacemaker pulse to correct any synchronized objects
    //! that may have fallen out of sync due to lag or network delays. This method sets N.
    //! N must be at least 2, and also for technical reasons it has to be an even number.
    virtual void setPacemakerPulsePeriod(hg::PZInteger aPeriod) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // MISC.                                                                 //
    ///////////////////////////////////////////////////////////////////////////

    // TODO - Not gonna be correct anymore (use lobby mapping!)
    virtual int getLocalPlayerIndex() = 0;

private:
    SPEMPE_CTXCOMP_TAG("jbatnozic::spempe::NetworkingManagerInterface");
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_NETWORKING_MANAGER_INTERFACE_HPP
