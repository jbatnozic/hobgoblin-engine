// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef SPEMPE_MANAGERS_INPUT_SYNC_MANAGER_DEFAULT_HPP
#define SPEMPE_MANAGERS_INPUT_SYNC_MANAGER_DEFAULT_HPP

#include <Hobgoblin/Utility/Packet.hpp>
#include <Hobgoblin/Utility/State_scheduler_simple.hpp>
#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>
#include <SPeMPE/Managers/Input_sync_manager_interface.hpp>

#include <unordered_map>
#include <variant>

namespace jbatnozic {
namespace spempe {

class DefaultInputSyncManager 
    : public InputSyncManagerInterface
    , public NonstateObject {
public:
    DefaultInputSyncManager(hg::QAO_RuntimeRef aRuntimeRef, int aExecutionPriority);

    void setToHostMode(hg::PZInteger aClientCount, hg::PZInteger aStateBufferingLength) override;
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

    void triggerEvent(std::string aEventName) override;
    
    void triggerEventWithPayload(std::string aEventName,
                                 const std::function<void(hg::util::Packet&)>& f) override;
    
    ///////////////////////////////////////////////////////////////////////////
    // SETTING INPUT VALUES (SERVER-SIDE)                                    //
    ///////////////////////////////////////////////////////////////////////////

    void setSignalValue(int aForClient, 
                        std::string aSignalName,
                        const std::function<void(hg::util::Packet&)>& f) override;

    void triggerEvent(int aForClient, std::string aEventName) override;

    void triggerEventWithPayload(int aForClient, 
                                 std::string aEventName,
                                 const std::function<void(hg::util::Packet&)>& f) override;

    ///////////////////////////////////////////////////////////////////////////
    // GETTING INPUT VALUES (SERVER-SIDE)                                    //
    ///////////////////////////////////////////////////////////////////////////

    void getSignalValue(int aForClient,
                        std::string aSignalName,
                        hg::util::Packet& aPacket) const override;

    void pollSimpleEvent(int aForClient,
                         std::string aEventName,
                         const std::function<void()>& aHandler) const override;

    void pollEventWithPayload(int aForClient,
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

    void _eventBeginUpdate() override;
    void _eventUpdate1() override;
    void _eventEndUpdate() override;

    friend void USPEMPE_DefaultInputSyncManager_PutNewState(DefaultInputSyncManager&,
                                                            int,
                                                            const hg::util::Packet&,
                                                            hg::PZInteger);
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_INPUT_SYNC_MANAGER_DEFAULT_HPP

// clang-format on
