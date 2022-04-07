#ifndef SPEMPE_MANAGERS_INPUT_SYNC_MANAGER_ONE_HPP
#define SPEMPE_MANAGERS_INPUT_SYNC_MANAGER_ONE_HPP

#include <Hobgoblin/Utility/Packet.hpp>
#include <Hobgoblin/Utility/State_scheduler_simple.hpp>
#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>
#include <SPeMPE/Managers/Input_sync_manager_interface.hpp>

#include <unordered_map>
#include <variant>

namespace jbatnozic {
namespace spempe {

class InputSyncManagerOne 
    : public InputSyncManagerInterface
    , public NonstateObject {
public:
    InputSyncManagerOne(hg::QAO_RuntimeRef aRuntimeRef, int aExecutionPriority);

    void setToHostMode(hg::PZInteger aPlayerCount, hg::PZInteger aStateBufferingLength) override;
    void setToClientMode() override;

    void setStateBufferingLength(hg::PZInteger aNewStateBufferingLength) override;

    ///////////////////////////////////////////////////////////////////////////
    // INPUT DEFINITIONS                                                     //
    ///////////////////////////////////////////////////////////////////////////

    void defineSignal(std::string aSignalName, 
                      const std::type_info& aSignalType,
                      const hg::util::Packet& aInitialValue) override;

    void defineSimpleEvent(std::string aEventName) override;

    void defineEventWithPayload(std::string aEventName, const std::type_info& aPayloadType) override;

    const std::type_info& getSignalType(std::string aSignalName) const override;

    const std::type_info& getEventPayloadType(std::string aEventName) const override;

    ///////////////////////////////////////////////////////////////////////////
    // SETTING INPUT VALUES (CLIENT-SIDE)                                    //
    ///////////////////////////////////////////////////////////////////////////

    void setSignalValue(std::string aSignalName,
                        const std::function<void(hg::util::Packet&)>& f) override;

    void setSignalValue(hg::PZInteger aForPlayer, 
                        std::string aSignalName,
                        const std::function<void(hg::util::Packet&)>& f) override;

    void triggerEvent(std::string aEventName) override;

    void triggerEvent(hg::PZInteger aForPlayer, std::string aEventName) override;

    void triggerEventWithPayload(std::string aEventName,
                                 const std::function<void(hg::util::Packet&)>& f) override;

    void triggerEventWithPayload(hg::PZInteger aForPlayer, 
                                 std::string aEventName,
                                 const std::function<void(hg::util::Packet&)>& f) override;

    ///////////////////////////////////////////////////////////////////////////
    // GETTING INPUT VALUES (SERVER-SIDE)                                    //
    ///////////////////////////////////////////////////////////////////////////

    void getSignalValue(hg::PZInteger aForPlayer,
                        std::string aSignalName,
                        hg::util::Packet& aPacket) const override;

    void pollSimpleEvent(hg::PZInteger aForPlayer,
                         std::string aEventName,
                         const std::function<void()>& aHandler) const override;

    void pollEventWithPayload(hg::PZInteger aForPlayer,
                              std::string aEventName,
                              const std::function<void(hg::util::Packet&)>& aPayloadHandler) const override;

private:
    enum class Mode {
        Default,
        Host,
        Client
    };

    Mode _mode = Mode::Default;

    struct SignalElem {
        const std::type_info& signalType;
        hg::util::Packet value;

        SignalElem(const std::type_info& aSignalType, const hg::util::Packet& aInitialValue)
            : signalType{aSignalType}
            , value{aInitialValue}
        {
        }
    };

    struct SimpleEventElem {
        hg::PZInteger count = 0;
    };

    struct EventWithPayloadElem {
        const std::type_info& payloadType;
        hg::util::Packet payloads;
        hg::PZInteger count = 0;

        EventWithPayloadElem(const std::type_info& aSignalType) : payloadType{aSignalType} {}
    };

    using InputVariant = std::variant<std::monostate, SignalElem, SimpleEventElem, EventWithPayloadElem>;
    using InputMap     = std::unordered_map<std::string, InputVariant>;

    std::vector<InputMap> _maps;

    std::vector<hg::util::SimpleStateScheduler<hg::util::Packet>> _incomingStates;

    mutable hg::util::Packet _helperPacket;

    void _packSingleState(hg::PZInteger aIndex, hg::util::Packet& packet);
    void _unpackSingleState(hg::PZInteger aIndex, hg::util::Packet& packet);
    void _clearAllEvents(hg::PZInteger aIndex);

    void _eventPreUpdate() override;
    void _eventUpdate() override;
    void _eventPostUpdate() override;

    friend void PutNewState(InputSyncManagerOne&, hg::PZInteger, const hg::util::Packet&, hg::PZInteger);
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_INPUT_SYNC_MANAGER_ONE_HPP