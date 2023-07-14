#ifndef SPEMPE_MANAGERS_NETWORKING_MANAGER_DEFAULT_HPP
#define SPEMPE_MANAGERS_NETWORKING_MANAGER_DEFAULT_HPP

#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>
#include <SPeMPE/Managers/Networking_manager_interface.hpp>

#include <deque>
#include <memory>
#include <vector>

namespace jbatnozic {
namespace spempe {

//! One concrete implementation of NetworkingManagerInterface.
//! TODO: Tip on setting execution priority?
class DefaultNetworkingManager 
    : public NetworkingManagerInterface
    , public NonstateObject {
public:
    DefaultNetworkingManager(hg::QAO_RuntimeRef aRuntimeRef,
                             int aExecutionPriority,
                             hg::PZInteger aStateBufferingLength);

    ///////////////////////////////////////////////////////////////////////////
    // CONFIGURATION                                                         //
    ///////////////////////////////////////////////////////////////////////////

    void setToServerMode(hg::RN_Protocol aProtocol,
                         std::string aPassphrase,
                         hg::PZInteger aServerSize,
                         hg::PZInteger aMaxPacketSize,
                         hg::RN_NetworkingStack aNetworkingStack) override;


    void setToClientMode(hg::RN_Protocol aProtocol,
                         std::string aPassphrase,
                         hg::PZInteger aMaxPacketSize,
                         hg::RN_NetworkingStack aNetworkingStack) override;

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

    RegistryId getRegistryId() override;

    hg::PZInteger getStateBufferingLength() const override;

    void setStateBufferingLength(hg::PZInteger aNewStateBufferingLength) override;

    void setPacemakerPulsePeriod(hg::PZInteger aPeriod) override;

    void setAutomaticStateSyncForNewConnectionsEnabled(bool aEnabled) override;

    void syncCompleteStateToClient(hg::PZInteger aClientIndex, bool aCleanFirst) override;

    ///////////////////////////////////////////////////////////////////////////
    // TELEMETRY                                                             //
    ///////////////////////////////////////////////////////////////////////////

    void setTelemetryCycleLimit(hg::PZInteger aCycleLimit) override;

    hg::PZInteger getTelemetryUploadByteCount(hg::PZInteger aCycleCount) const override;

    hg::PZInteger getTelemetryDownloadByteCount(hg::PZInteger aCycleCount) const override;

    ///////////////////////////////////////////////////////////////////////////
    // MISC.                                                                 //
    ///////////////////////////////////////////////////////////////////////////

    int getLocalClientIndex() const override;

protected:
    void _eventStartFrame() override;
    void _eventPreUpdate() override;
    void _eventPostUpdate() override;

private:
    Mode _mode = Mode::Uninitialized;
    int _localClientIndex = CLIENT_INDEX_UNKNOWN;

    std::unique_ptr<NodeType> _node;
    detail::SynchronizedObjectRegistry _syncObjReg;

    std::vector<NetworkingEventListener*> _eventListeners;

    struct TelemetryCycleInfo {
        hg::PZInteger uploadByteCount   = 0;
        hg::PZInteger downloadByteCount = 0;
    };
    std::deque<TelemetryCycleInfo> _telemetry;

    void _handleEvents();
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_NETWORKING_MANAGER_DEFAULT_HPP
