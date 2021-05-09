#ifndef SPEMPE_MANAGERS_NETWORKING_MANAGER_ONE_HPP
#define SPEMPE_MANAGERS_NETWORKING_MANAGER_ONE_HPP

#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>
#include <SPeMPE/Managers/Networking_manager_interface.hpp>

#include <memory>
#include <vector>

namespace jbatnozic {
namespace spempe {

//! One concrete implementation of NetworkingManagerInterface.
//! TODO: Tip on setting execution priority?
class NetworkingManagerOne 
    : public NetworkingManagerInterface
    , public NonstateObject {
public:
    NetworkingManagerOne(hg::QAO_RuntimeRef aRuntimeRef,
                         int aExecutionPriority);

    ///////////////////////////////////////////////////////////////////////////
    // CONFIGURATION                                                         //
    ///////////////////////////////////////////////////////////////////////////

    void setToMode(Mode aMode) override;

    Mode getMode() const override;

    bool isUninitialized() const override;
    bool isServer() const override;
    bool isClient() const override;

    ///////////////////////////////////////////////////////////////////////////
    // NODE ACCESS                                                           //
    ///////////////////////////////////////////////////////////////////////////

    NodeType& getNode() const override;

    ServerType& getServer() const override;

    ClientType& getClient() const override;

    ///////////////////////////////////////////////////////////////////////////
    // LISTENER MANAGEMENT                                                   //
    ///////////////////////////////////////////////////////////////////////////

    void addEventListener(NetworkingEventListener& aListener) override;

    void removeEventListener(NetworkingEventListener& aListener) override;

    ///////////////////////////////////////////////////////////////////////////
    // SYNCHRONIZATION                                                       //
    ///////////////////////////////////////////////////////////////////////////

    SynchronizedObjectRegistry& getSyncObjReg() override;

    ///////////////////////////////////////////////////////////////////////////
    // MISC.                                                                 //
    ///////////////////////////////////////////////////////////////////////////

    int getLocalPlayerIndex() override;

protected:
    void eventPreUpdate() override;
    void eventPostUpdate() override;

private:
    Mode _mode = Mode::Uninitialized;
    int _localPlayerIndex = PLAYER_INDEX_NONE;

    std::unique_ptr<NodeType> _node;
    SynchronizedObjectRegistry _syncObjReg;

    std::vector<NetworkingEventListener*> _eventListeners;

    void _handleEvents();
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_NETWORKING_MANAGER_ONE_HPP
