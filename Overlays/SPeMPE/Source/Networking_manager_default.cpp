// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <SPeMPE/Managers/Networking_manager_default.hpp>

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Logging.hpp>

#include <algorithm>
#include <cassert>
#include <type_traits>

namespace jbatnozic {
namespace spempe {
namespace {
constexpr const char* LOG_ID = "SPeMPE";
} // namespace

DefaultNetworkingManager::DefaultNetworkingManager(hg::QAO_RuntimeRef aRuntimeRef,
                                                   int aExecutionPriority,
                                                   hg::PZInteger aStateBufferingLength)
    : NonstateObject{aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "::jbatnozic::spempe::DefaultNetworkingManager"}
    , _node{hg::RN_ServerFactory::createDummyServer()}
    , _syncObjReg{*_node, aStateBufferingLength}
{
}

///////////////////////////////////////////////////////////////////////////
// CONFIGURATION                                                         //
///////////////////////////////////////////////////////////////////////////

void DefaultNetworkingManager::setToServerMode(hg::RN_Protocol aProtocol,
                                               std::string aPassphrase,
                                               hg::PZInteger aServerSize,
                                               hg::PZInteger aMaxPacketSize,
                                               hg::RN_NetworkingStack aNetworkingStack) {
    assert(_mode == Mode::Uninitialized);

    SPEMPE_VALIDATE_GAME_CONTEXT_FLAGS(ctx(), privileged==true, networking==true);
    _localClientIndex = CLIENT_INDEX_LOCAL;
    _node = hg::RN_ServerFactory::createServer(
        aProtocol,
        std::move(aPassphrase),
        aServerSize,
        aMaxPacketSize,
        aNetworkingStack
    );
    _node->setUserData(&ctx());
    _syncObjReg.setNode(*_node);

    _mode = Mode::Server;
}


void DefaultNetworkingManager::setToClientMode(hg::RN_Protocol aProtocol,
                                               std::string aPassphrase,
                                               hg::PZInteger aMaxPacketSize,
                                               hg::RN_NetworkingStack aNetworkingStack) {
    assert(_mode == Mode::Uninitialized);

    SPEMPE_VALIDATE_GAME_CONTEXT_FLAGS(ctx(), privileged==false, networking==true);
    _localClientIndex = CLIENT_INDEX_UNKNOWN;
    _node = hg::RN_ClientFactory::createClient(
        aProtocol,
        std::move(aPassphrase),
        aMaxPacketSize,
        aNetworkingStack
    );
    _node->setUserData(&ctx());
    _syncObjReg.setNode(*_node);

    _mode = Mode::Client;
}

NetworkingManagerInterface::Mode DefaultNetworkingManager::getMode() const {
    return _mode;
}

bool DefaultNetworkingManager::isUninitialized() const {
    return _mode == Mode::Uninitialized;
}

bool DefaultNetworkingManager::isServer() const {
    return _mode == Mode::Server;
}

bool DefaultNetworkingManager::isClient() const {
    return _mode == Mode::Client;
}

///////////////////////////////////////////////////////////////////////////
// NODE ACCESS                                                           //
///////////////////////////////////////////////////////////////////////////

NetworkingManagerInterface::NodeType& DefaultNetworkingManager::getNode() const {
    return *_node;
}

NetworkingManagerInterface::ServerType& DefaultNetworkingManager::getServer() const {
    assert(isServer());
    return static_cast<ServerType&>(getNode());
}

NetworkingManagerInterface::ClientType& DefaultNetworkingManager::getClient() const {
    assert(isClient());
    return static_cast<ClientType&>(getNode());
}

///////////////////////////////////////////////////////////////////////////
// LISTENER MANAGEMENT                                                   //
///////////////////////////////////////////////////////////////////////////

void DefaultNetworkingManager::addEventListener(NetworkingEventListener& aListener) {
    for (const auto listener : _eventListeners) {
        if (listener == &aListener) {
            return;
        }
    }
    _eventListeners.push_back(&aListener);
}

void DefaultNetworkingManager::removeEventListener(NetworkingEventListener& aListener) {
    _eventListeners.erase(
        std::remove_if(_eventListeners.begin(), _eventListeners.end(),
                       [&aListener](const NetworkingEventListener* aCurr) {
                           return aCurr == &aListener;
                       }), _eventListeners.end());
}

///////////////////////////////////////////////////////////////////////////
// SYNCHRONIZATION                                                       //
///////////////////////////////////////////////////////////////////////////

RegistryId DefaultNetworkingManager::getRegistryId() {
    return {
        reinterpret_cast< decltype(std::declval<RegistryId>().address) >(&_syncObjReg)
    };
}

hg::PZInteger DefaultNetworkingManager::getStateBufferingLength() const {
    return _syncObjReg.getDefaultDelay();
}

void DefaultNetworkingManager::setStateBufferingLength(hg::PZInteger aNewStateBufferingLength) {
    _syncObjReg.setDefaultDelay(aNewStateBufferingLength);
}

void DefaultNetworkingManager::setPacemakerPulsePeriod(hg::PZInteger aPeriod) {
    _syncObjReg.setPacemakerPulsePeriod(aPeriod);
}

void DefaultNetworkingManager::setAutomaticStateSyncForNewConnectionsEnabled(bool aEnabled) {
    // TODO
}

void DefaultNetworkingManager::syncCompleteStateToClient(hg::PZInteger aClientIndex, bool aCleanFirst) {
    // TODO
}

///////////////////////////////////////////////////////////////////////////
// TELEMETRY                                                             //
///////////////////////////////////////////////////////////////////////////

void DefaultNetworkingManager::setTelemetryCycleLimit(hg::PZInteger aCycleLimit) {
    while (hg::stopz(_telemetry.size()) < aCycleLimit) {
        _telemetry.emplace_front();
    }

    while (hg::stopz(_telemetry.size()) > aCycleLimit) {
        _telemetry.pop_front();
    }
}

hg::RN_Telemetry DefaultNetworkingManager::getTelemetry(hg::PZInteger aCycleCount) const {
    if (aCycleCount > hg::stopz(_telemetry.size())) {
        HG_THROW_TRACED(hg::TracedLogicError, 0,
                        "aCycleCount ({}) must not be greater than the telemetry cycle limit ({}).",
                        aCycleCount, _telemetry.size());
    }

    hg::RN_Telemetry result;
    for ( int i = static_cast<int>(_telemetry.size()) - 1
        ; i > static_cast<int>(_telemetry.size()) - 1 - aCycleCount
        ; i -= 1
        ) {
        result += _telemetry[static_cast<std::size_t>(i)];
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////
// MISC.                                                                 //
///////////////////////////////////////////////////////////////////////////

int DefaultNetworkingManager::getLocalClientIndex() const {
    return _localClientIndex;
}

///////////////////////////////////////////////////////////////////////////
// PROTECTED & PRIVATE METHODS                                           //
///////////////////////////////////////////////////////////////////////////

void DefaultNetworkingManager::_eventPreUpdate() {
    _telemetry.emplace_back();
    _telemetry.pop_front();
}

void DefaultNetworkingManager::_eventBeginUpdate() {
    const auto telemetry = _node->update(hg::RN_UpdateMode::Receive);
    if (!_telemetry.empty()) {
        _telemetry.back() += telemetry;
    }
    _handleEvents();
}

void DefaultNetworkingManager::_eventEndUpdate() {
    // Update all Synchronized objects
    if (_node->isServer()) {
        _syncObjReg.syncStateUpdates();
    }

    const auto telemetry = _node->update(hg::RN_UpdateMode::Send);
    if (!_telemetry.empty()) {
        _telemetry.back() += telemetry;
    }
    _handleEvents();
}

void DefaultNetworkingManager::_handleEvents() {
    using hg::RN_Event;

    RN_Event ev;

    while (_node->pollEvent(ev)) {
        ev.strictVisit(
            [](const RN_Event::BadPassphrase&) {
                HG_LOG_INFO(LOG_ID, "Received event: Bad passphrase.");
            },
            [](const RN_Event::ConnectAttemptFailed&) {
                HG_LOG_INFO(LOG_ID, "Received event: Connection attempt failed ({}).", /*ev.reason*/ "?");
            },
            [this](const RN_Event::Connected& aEventData) {
                if (_node->isServer()) {
                    HG_LOG_INFO(LOG_ID, "Received event: New client ({}) connected.", *aEventData.clientIndex);
                    _syncObjReg.syncCompleteState(*aEventData.clientIndex);
                }
                else {
                    HG_LOG_INFO(LOG_ID, "Received event: Connected to server.");
                    _localClientIndex = getClient().getClientIndex();
                }
            },
            [this](const RN_Event::Disconnected& aEventData) {
                HG_LOG_INFO(LOG_ID, "Received event: Disconnect ({}).", aEventData.message);
                if (!_node->isServer()) {
                    _localClientIndex = CLIENT_INDEX_UNKNOWN;
                }
            }
        );

        for (auto& listener : _eventListeners) {
            listener->onNetworkingEvent(ev);
        }
    }
}

} // namespace spempe
} // namespace jbatnozic

// clang-format on
