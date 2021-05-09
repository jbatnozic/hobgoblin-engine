#ifndef SPEMPE_MANAGERS_NETWORKING_MANAGER_INTERFACE_HPP
#define SPEMPE_MANAGERS_NETWORKING_MANAGER_INTERFACE_HPP

#include <Hobgoblin/RigelNet.hpp>

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

    virtual SynchronizedObjectRegistry& getSyncObjReg() = 0;

    ///////////////////////////////////////////////////////////////////////////
    // MISC.                                                                 //
    ///////////////////////////////////////////////////////////////////////////

    virtual int getLocalPlayerIndex() = 0;

private:
    SPEMPE_CTXCOMP_TAG("spempe::NetworkingManagerInterface");
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_NETWORKING_MANAGER_INTERFACE_HPP
