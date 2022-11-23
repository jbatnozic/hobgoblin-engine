
#include <SPeMPE/Managers/Input_sync_manager_default.hpp>

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet_macros.hpp>
#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>

#include <cassert>

namespace jbatnozic {
namespace spempe {

namespace {

static constexpr char SIGNAL_TAG             = 'S';
static constexpr char SIMPLE_EVENT_TAG       = 'E';
static constexpr char EVENT_WITH_PAYLOAD_TAG = 'P';

} // namespace

using namespace hg::rn;

void USPEMPE_InputSyncManagerOne_PutNewState(DefaultInputSyncManager& aMgr,
                                             hg::PZInteger aForPlayer,
                                             const hg::util::Packet& aPacket,
                                             hg::PZInteger aDelay) {
    aMgr._incomingStates.at(hg::pztos(aForPlayer)).putNewState(aPacket, aDelay);
}

RN_DEFINE_RPC(USPEMPE_InputSyncManagerOne_SendInput, RN_ARGS(hg::util::Packet&, aPacket)) {
    RN_NODE_IN_HANDLER().callIfServer(
        [&](RN_ServerInterface& aServer) {
            const auto rc = SPEMPE_GET_RPC_RECEIVER_CONTEXT(aServer);
            rc.gameContext.getComponent<InputSyncManagerInterface>();
            USPEMPE_InputSyncManagerOne_PutNewState(
                static_cast<DefaultInputSyncManager&>(rc.gameContext.getComponent<InputSyncManagerInterface>()),
                rc.senderIndex + 1,
                aPacket,
                rc.pessimisticLatencyInSteps
            );
        });

    RN_NODE_IN_HANDLER().callIfClient(
        [](RN_ClientInterface& aClient) {
            throw RN_IllegalMessage();
        });
}

DefaultInputSyncManager::DefaultInputSyncManager(hg::QAO_RuntimeRef aRuntimeRef, int aExecutionPriority) 
    : NonstateObject{aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "::jbatnozic::spempe::DefaultInputSyncManager"}
{
}

void DefaultInputSyncManager::setToHostMode(hg::PZInteger aPlayerCount, hg::PZInteger aStateBufferingLength) {
    SPEMPE_VERIFY_GAME_CONTEXT_FLAGS(ctx(), privileged==true, networking==true);

    if (aPlayerCount < 1) {
        throw hg::TracedLogicError{"DefaultInputSyncManager - Player count must be at least 1!"};
    }

    _mode = Mode::Host;

    _maps.resize(hg::pztos(aPlayerCount));

    _incomingStates.reserve(hg::pztos(aPlayerCount));
    for (hg::PZInteger i = 0; i < aPlayerCount; i += 1) {
        _incomingStates.emplace_back(aStateBufferingLength);
    }
}

void DefaultInputSyncManager::setToClientMode() {
    SPEMPE_VERIFY_GAME_CONTEXT_FLAGS(ctx(), privileged==false, networking==true);
    _mode = Mode::Client;
    _maps.resize(1u);
}

void DefaultInputSyncManager::setStateBufferingLength(hg::PZInteger aNewStateBufferingLength) {
    for (auto& scheduler : _incomingStates) {
        scheduler.setDefaultDelay(aNewStateBufferingLength);
    }
}

///////////////////////////////////////////////////////////////////////////
// INPUT DEFINITIONS                                                     //
///////////////////////////////////////////////////////////////////////////

void DefaultInputSyncManager::defineSignal(std::string aSignalName, 
                                           const std::type_info& aSignalType,
                                           const hg::util::Packet& aInitialValue) {
    aSignalName += SIGNAL_TAG;

    for (auto& map : _maps) {
        if (map.find(aSignalName) != map.end()) {
            throw hg::TracedLogicError("DefaultInputSyncManager - defining same signal multiple times!");
        }
        map[aSignalName].emplace<SignalElem>(aSignalType, aInitialValue);
    }
}

void DefaultInputSyncManager::defineSimpleEvent(std::string aEventName) {
    aEventName += SIMPLE_EVENT_TAG;

    for (auto& map : _maps) {
        if (map.find(aEventName) != map.end()) {
            throw hg::TracedLogicError("DefaultInputSyncManager - defining same simple event multiple times!");
        }
        map[aEventName].emplace<SimpleEventElem>();
    }
}

void DefaultInputSyncManager::defineEventWithPayload(std::string aEventName, const std::type_info& aPayloadType) {
    aEventName += EVENT_WITH_PAYLOAD_TAG;

    for (auto& map : _maps) {
        if (map.find(aEventName) != map.end()) {
            throw hg::TracedLogicError("DefaultInputSyncManager - defining same event with payload multiple times!");
        }
        map[aEventName].emplace<EventWithPayloadElem>(aPayloadType);
    }
}

const std::type_info& DefaultInputSyncManager::getSignalType(std::string aSignalName) const {
    aSignalName += SIGNAL_TAG;
    auto& map = _maps[0];

    const auto iter = map.find(aSignalName);
    if (iter == map.end()) {
        throw hg::TracedLogicError{"DefaultInputSyncManager - Requested signal not found!"};
    }

    return std::get<SignalElem>(iter->second).signalType;
}

const std::type_info& DefaultInputSyncManager::getEventPayloadType(std::string aEventName) const {
    aEventName += EVENT_WITH_PAYLOAD_TAG;
    auto& map = _maps[0];

    const auto iter = map.find(aEventName);
    if (iter == map.end()) {
        throw hg::TracedLogicError{"DefaultInputSyncManager - Requested event with payload not found!"};
    }

    return std::get<EventWithPayloadElem>(iter->second).payloadType;
}

///////////////////////////////////////////////////////////////////////////
// SETTING INPUT VALUES (CLIENT-SIDE)                                    //
///////////////////////////////////////////////////////////////////////////

void DefaultInputSyncManager::setSignalValue(std::string aSignalName,
                                             const std::function<void(hg::util::Packet&)>& f) {
    if (_mode != Mode::Client) {
        throw hg::TracedLogicError{"DefaultInputSyncManager - This overload of setSignalValue "
                                   "is for client configuration only!"};
    }

    auto& map = _maps[0];

    aSignalName += SIGNAL_TAG;
    const auto iter = map.find(aSignalName);
    if (iter == map.end()) {
        throw hg::TracedLogicError("DefaultInputSyncManager - Requested signal not found!");
    }

    auto& packet = std::get<SignalElem>(iter->second).value;
    packet.clear();
    f(packet);
}

void DefaultInputSyncManager::setSignalValue(hg::PZInteger aForPlayer, 
                                             std::string aSignalName,
                                             const std::function<void(hg::util::Packet&)>& f) {
    if (_mode != Mode::Host) {
        throw hg::TracedLogicError{"DefaultInputSyncManager - This overload of setSignalValue "
                                   "is for host configuration only!"};
    }

    auto& map = _maps.at(aForPlayer);

    aSignalName += SIGNAL_TAG;
    const auto iter = map.find(aSignalName);
    if (iter == map.end()) {
        throw hg::TracedLogicError("DefaultInputSyncManager - Requested signal not found!");
    }

    auto& packet = std::get<SignalElem>(iter->second).value;
    packet.clear();
    f(packet);
}

void DefaultInputSyncManager::triggerEvent(std::string aEventName) {
    if (_mode != Mode::Client) {
        throw hg::TracedLogicError{"DefaultInputSyncManager - This overload of setSignalValue "
                                   "is for client configuration only!"};
    }

    auto& map = _maps[0];

    aEventName += SIMPLE_EVENT_TAG;
    const auto iter = map.find(aEventName);
    if (iter == map.end()) {
        throw hg::TracedLogicError("DefaultInputSyncManager - Requested simple event not found!");
    }

    std::get<SimpleEventElem>(iter->second).count += 1;
}

void DefaultInputSyncManager::triggerEvent(hg::PZInteger aForPlayer,
                                           std::string aEventName) {
    if (_mode != Mode::Host) {
        throw hg::TracedLogicError{"DefaultInputSyncManager - This overload of setSignalValue "
                                   "is for host configuration only!"};
    }

    auto& map = _maps.at(aForPlayer);

    aEventName += SIMPLE_EVENT_TAG;
    const auto iter = map.find(aEventName);
    if (iter == map.end()) {
        throw hg::TracedLogicError("DefaultInputSyncManager - Requested simple event not found!");
    }

    std::get<SimpleEventElem>(iter->second).count += 1;
}

void DefaultInputSyncManager::triggerEventWithPayload(std::string aEventName,
                                                      const std::function<void(hg::util::Packet&)>& f) {
    if (_mode != Mode::Client) {
        throw hg::TracedLogicError{"DefaultInputSyncManager - This overload of setSignalValue "
                                   "is for client configuration only!"};
    }

    auto& map = _maps[0];

    aEventName += EVENT_WITH_PAYLOAD_TAG;
    const auto iter = map.find(aEventName);
    if (iter == map.end()) {
        throw hg::TracedLogicError("DefaultInputSyncManager - Requested event with payload not found!");
    }

    _helperPacket.clear();
    f(_helperPacket);

    auto& elem = std::get<EventWithPayloadElem>(iter->second);
    elem.count += 1;
    elem.payloads << _helperPacket;
}

void DefaultInputSyncManager::triggerEventWithPayload(hg::PZInteger aForPlayer, 
                                                      std::string aEventName,
                                                      const std::function<void(hg::util::Packet&)>& f) {
    if (_mode != Mode::Host) {
        throw hg::TracedLogicError{"DefaultInputSyncManager - This overload of setSignalValue "
                                   "is for host configuration only!"};
    }

    auto& map = _maps.at(aForPlayer);

    aEventName += EVENT_WITH_PAYLOAD_TAG;
    const auto iter = map.find(aEventName);
    if (iter == map.end()) {
        throw hg::TracedLogicError("DefaultInputSyncManager - Requested event with payload not found!");
    }

    _helperPacket.clear();
    f(_helperPacket);

    auto& elem = std::get<EventWithPayloadElem>(iter->second);
    elem.count += 1;
    elem.payloads << _helperPacket;
}

///////////////////////////////////////////////////////////////////////////
// GETTING INPUT VALUES (SERVER-SIDE)                                    //
///////////////////////////////////////////////////////////////////////////

void DefaultInputSyncManager::getSignalValue(hg::PZInteger aForPlayer,
                                             std::string aSignalName,
                                             hg::util::Packet& aPacket) const {
    auto& map = _maps.at(hg::pztos(aForPlayer));

    aSignalName += SIGNAL_TAG;
    const auto iter = map.find(aSignalName);
    if (iter == map.end()) {
        throw hg::TracedLogicError("DefaultInputSyncManager - Requested signal not found!");
    }

    aPacket = std::get<SignalElem>(iter->second).value; // TODO what if it was empty?
}

void DefaultInputSyncManager::pollSimpleEvent(hg::PZInteger aForPlayer,
                                              std::string aEventName,
                                              const std::function<void()>& aHandler) const {
    auto& map = _maps.at(hg::pztos(aForPlayer));

    aEventName += SIMPLE_EVENT_TAG;
    const auto iter = map.find(aEventName);
    if (iter == map.end()) {
        throw hg::TracedLogicError("DefaultInputSyncManager - Requested simple event not found!");
    }

    const auto& elem = std::get<SimpleEventElem>(iter->second);

    for (hg::PZInteger i = 0; i < elem.count; i += 1) {
        aHandler();
    }
}

void DefaultInputSyncManager::pollEventWithPayload(hg::PZInteger aForPlayer,
                                               std::string aEventName,
                                               const std::function<void(hg::util::Packet&)>& aPayloadHandler) const {
    auto& map = _maps.at(hg::pztos(aForPlayer));

    aEventName += EVENT_WITH_PAYLOAD_TAG;
    const auto iter = map.find(aEventName);
    if (iter == map.end()) {
        throw hg::TracedLogicError("DefaultInputSyncManager - Requested event with payload not found!");
    }

    const auto& elem = std::get<EventWithPayloadElem>(iter->second);
    hg::util::Packet payloadsCopy = elem.payloads;

    for (hg::PZInteger i = 0; i < elem.count; i += 1) {
        payloadsCopy >> _helperPacket;
        aPayloadHandler(_helperPacket);
    }
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS                                                       //
///////////////////////////////////////////////////////////////////////////

void DefaultInputSyncManager::_packSingleState(hg::PZInteger aIndex, hg::util::Packet& packet) {
    // 1. # of embedded inputs
    // 2. inputs:
    //   - signals (name, value)
    //   - simple events (name, count)
    //   - events with payload (name, count, payload)

    auto& map = _maps.at(hg::pztos(aIndex));

    packet << static_cast<std::uint16_t>(map.size());

    for (auto& pair : map) {
        switch (pair.first.back()) {
        case SIGNAL_TAG:
            packet << pair.first << std::get<SignalElem>(pair.second).value;
            break;

        case SIMPLE_EVENT_TAG:
            packet << pair.first << std::get<SimpleEventElem>(pair.second).count;
            break;

        case EVENT_WITH_PAYLOAD_TAG:
            packet << pair.first << std::get<EventWithPayloadElem>(pair.second).count 
                   << std::get<EventWithPayloadElem>(pair.second).payloads;
            break;

        default:
            assert(false && "Unreachable");
            break;
        }
    }
}

void DefaultInputSyncManager::_unpackSingleState(hg::PZInteger aIndex, hg::util::Packet& packet) {
    auto& map = _maps.at(hg::pztos(aIndex));

    std::uint16_t count;
    packet >> count;

    std::string name;

    for (std::uint16_t i = 0; i < count; i += 1) {
        packet >> name;
        const auto iter = map.find(name);
        if (iter == map.end()) {
            throw hg::TracedRuntimeError{"DefaultInputSyncManager - Unpacking unrecognised entry!"};
        }

        switch (name.back()) {
        case SIGNAL_TAG:
            {
                auto& elem = std::get<SignalElem>(iter->second);
                packet >> elem.value;
            }
            break;

        case SIMPLE_EVENT_TAG:
            {
                auto& elem = std::get<SimpleEventElem>(iter->second);
                packet >> elem.count;
            }
            break;

        case EVENT_WITH_PAYLOAD_TAG:
            {
                auto& elem = std::get<EventWithPayloadElem>(iter->second);
                packet >> elem.count >> elem.payloads;
            }
            break;

        default:
            assert(false && "Unreachable");
            break;
        }
    }
}

void DefaultInputSyncManager::_clearAllEvents(hg::PZInteger aIndex) {
    auto& map = _maps.at(hg::pztos(aIndex));

    for (auto& pair : map) {
        switch (pair.first.back()) {
        case SIGNAL_TAG:
            break;

        case SIMPLE_EVENT_TAG:
            {
                auto& elem = std::get<SimpleEventElem>(pair.second);
                elem.count = 0;
            }
            break;

        case EVENT_WITH_PAYLOAD_TAG:
            {
                auto& elem = std::get<EventWithPayloadElem>(pair.second);
                elem.count = 0;
                elem.payloads.clear();
            }
            break;

        default:
            assert(false && "Unreachable");
            break;
        }
    }
}

void DefaultInputSyncManager::_eventPreUpdate() {
    // If Host, apply new input
    if (_mode == Mode::Host) {

        for (std::size_t i = 0; i < _incomingStates.size(); i += 1) {
            _incomingStates[i].scheduleNewStates();

            auto& newState = _incomingStates[i].getCurrentState();
            bool  newStateFresh = _incomingStates[i].isCurrentStateFresh();
            // DataSize count be 0 if nothing was received yet from the client; 
            // In that case the state scheduler is just juggling empty packets
            if (newState.getDataSize() > 0 && !newState.endOfPacket()) {
                _unpackSingleState(hg::stopz(i), newState);
            }
            // Old events must not be repeated
            if (!newStateFresh) {
                _clearAllEvents(hg::stopz(i));
            }
        }

    }
}

void DefaultInputSyncManager::_eventUpdate() {
    // If Client, send all inputs
    if (_mode == Mode::Client) {

        _helperPacket.clear();
        _packSingleState(0, _helperPacket);
        _clearAllEvents(0);

        auto& node = ccomp<NetworkingManagerInterface>().getNode(); // TODO Temp.

        Compose_USPEMPE_InputSyncManagerOne_SendInput(node, RN_COMPOSE_FOR_ALL, _helperPacket);
    }
}

void DefaultInputSyncManager::_eventPostUpdate() {
    // If Host, advance all state schedulers
    if (_mode == Mode::Host) {

        for (std::size_t i = 0; i < _incomingStates.size(); i += 1) {
            _incomingStates[i].advance();
        }

    }
}

} // namespace spempe
} // namespace jbatnozic