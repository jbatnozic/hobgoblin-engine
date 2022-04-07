#ifndef SPEMPE_MANAGERS_INPUT_SYNC_MANAGER_INTERFACE_HPP
#define SPEMPE_MANAGERS_INPUT_SYNC_MANAGER_INTERFACE_HPP

#include <Hobgoblin/RigelNet/Node_interface.hpp>
#include <Hobgoblin/Utility/Packet.hpp>
#include <SPeMPE/GameContext/Context_components.hpp>
#include <SPeMPE/GameObjectFramework/Synchronized_object_registry.hpp>

#include <functional>
#include <string>
#include <typeinfo>

namespace jbatnozic {
namespace spempe {

/*            WindowMgr     ->   ControlsMgr   ->    InputSyncMgr
FrameStart:  records input           -                    -
PreUpdate:                                           [s] receives input
Update:
PostUpdate:                                          [c] uploads input
*/

class InputSyncManagerInterface : public ContextComponent {
public:
    virtual void setToHostMode(hg::PZInteger aPlayerCount, hg::PZInteger aStateBufferingLength) = 0;

    virtual void setToClientMode() = 0;

    virtual void setStateBufferingLength(hg::PZInteger aNewStateBufferingLength) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // INPUT DEFINITIONS                                                     //
    ///////////////////////////////////////////////////////////////////////////

    virtual void defineSignal(std::string aSignalName, 
                              const std::type_info& aSignalType,
                              const hg::util::Packet& aInitialValue) = 0;

    virtual void defineSimpleEvent(std::string aEventName) = 0;

    virtual void defineEventWithPayload(std::string aEventName, const std::type_info& aPayloadType) = 0;

    virtual const std::type_info& getSignalType(std::string aSignalName) const = 0;

    virtual const std::type_info& getEventPayloadType(std::string aEventName) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    // SETTING INPUT VALUES (CLIENT-SIDE)                                    //
    ///////////////////////////////////////////////////////////////////////////

    virtual void setSignalValue(std::string aSignalName,
                                const std::function<void(hg::util::Packet&)>& f) = 0;

    virtual void setSignalValue(hg::PZInteger aForPlayer,
                                std::string aSignalName,
                                const std::function<void(hg::util::Packet&)>& f) = 0;

    virtual void triggerEvent(std::string aEventName) = 0;

    virtual void triggerEvent(hg::PZInteger aForPlayer, std::string aEventName) = 0;

    virtual void triggerEventWithPayload(std::string aEventName,
                                         const std::function<void(hg::util::Packet&)>& f) = 0;

    virtual void triggerEventWithPayload(hg::PZInteger aForPlayer, 
                                         std::string aEventName,
                                         const std::function<void(hg::util::Packet&)>& f) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // GETTING INPUT VALUES (SERVER-SIDE)                                    //
    ///////////////////////////////////////////////////////////////////////////

    virtual void getSignalValue(hg::PZInteger aForPlayer,
                                std::string aSignalName,
                                hg::util::Packet& aPacket) const = 0;

    virtual void pollSimpleEvent(hg::PZInteger aForPlayer,
                                 std::string aEventName,
                                 const std::function<void()>& aHandler) const = 0;

    virtual void pollEventWithPayload(hg::PZInteger aForPlayer,
                                      std::string aEventName,
                                      const std::function<void(hg::util::Packet&)>& aPayloadHandler) const = 0;

private:
    SPEMPE_CTXCOMP_TAG("jbatnozic::spempe::InputSyncManager");
};

class InputSyncManagerWrapper {
public:
    InputSyncManagerWrapper(InputSyncManagerInterface& aMgr)
        : _mgr{aMgr}
    {
    }

    template <class taSignalType>
    void defineSignal(std::string aSignalName, const taSignalType& aInitialValue) const {
        _helperPacket.clear();
        _helperPacket << aInitialValue;
        _mgr.defineSignal(std::move(aSignalName), typeid(taSignalType), _helperPacket);
        _helperPacket.clear();
    }

    void defineSimpleEvent(std::string aEventName) const {
        _mgr.defineSimpleEvent(std::move(aEventName));
    }

    template <class taEventType>
    void defineEventWithPayload(std::string aEventName) const {
        _mgr.defineEventWithPayload(std::move(aEventName), typeid(taEventType));
    }

    template <class taSignalType>
    void setSignalValue(std::string aSignalName, const taSignalType& aValue) const {
        if (typeid(taSignalType) != _mgr.getSignalType(aSignalName)) {
            throw hg::TracedLogicError{"InputSyncManagerAdapter - Incorrect type for signal value!"};
        }
        _mgr.setSignalValue(std::move(aSignalName),
                            [&](hg::util::Packet& aPacket) {
                                aPacket << aValue;
                            });
    }

    template <class taSignalType>
    void setSignalValue(hg::PZInteger aForPlayer, std::string aSignalName, const taSignalType& aValue) const {
        if (typeid(taSignalType) != _mgr.getSignalType(aSignalName)) {
            throw hg::TracedLogicError{"InputSyncManagerAdapter - Incorrect type for signal value!"};
        }
        _mgr.setSignalValue(aForPlayer,
                            std::move(aSignalName),
                            [&](hg::util::Packet& aPacket) {
                                aPacket << aValue;
                            });
    }

    void triggerEvent(std::string aEventName, bool aPredicate = true) const {
        if (aPredicate) {
            _mgr.triggerEvent(std::move(aEventName));
        }
    }

    void triggerEvent(hg::PZInteger aForPlayer, std::string aEventName, bool aPredicate = true) const {
        if (aPredicate) {
            _mgr.triggerEvent(aForPlayer, std::move(aEventName));
        }
    }

    template <class taPayloadType>
    void triggerEventWithPayload(std::string aEventName, const taPayloadType& aPayload, bool aPredicate = true) const {
        if (typeid(taPayloadType) != _mgr.getEventPayloadType(aEventName)) {
            throw hg::TracedLogicError{"InputSyncManagerAdapter - Incorrect type for payload!"};
        }

        if (aPredicate) {
            _mgr.triggerEventWithPayload(std::move(aEventName),
                                         [&](hg::util::Packet& aPacket) {
                                             aPacket << aPayload;
                                         });
        }
    }

    template <class taSignalType>
    taSignalType getSignalValue(hg::PZInteger aForPlayer,
                                std::string aSignalName) const {
        if (typeid(taSignalType) != _mgr.getSignalType(aSignalName)) {
            throw hg::TracedLogicError{"InputSyncManagerAdapter - Incorrect type for signal value!"};
        }
        _helperPacket.clear();
        _mgr.getSignalValue(aForPlayer, std::move(aSignalName), _helperPacket);
        taSignalType retval;
        _helperPacket >> retval;
        _helperPacket.clear();
        return retval;
    }

    hg::PZInteger pollSimpleEvent(hg::PZInteger aForPlayer,
                                  std::string aEventName,
                                  const std::function<void()>& aHandler) const {
        hg::PZInteger count = 0;
        _mgr.pollSimpleEvent(aForPlayer, std::move(aEventName),
                             [&]() {
                                 count++;
                                 aHandler();
                             });
        return count;
    }

    hg::PZInteger countSimpleEvent(hg::PZInteger aForPlayer,
                          std::string aEventName) const {
        hg::PZInteger count = 0;
        _mgr.pollSimpleEvent(aForPlayer, 
                             std::move(aEventName), 
                             [&]() {
                                 count++;
                             });
        return count;
    }

    template <class taPayloadType>
    hg::PZInteger pollEventWithPayload(hg::PZInteger aForPlayer,
                                       std::string aEventName,
                                       const std::function<void(const taPayloadType&)>& aHandler) const {
        if (typeid(taPayloadType) != _mgr.getEventPayloadType(aEventName)) {
            throw hg::TracedLogicError{"InputSyncManagerAdapter - Incorrect type for payload!"};
        }

        hg::PZInteger count = 0;
        _mgr.pollEventWithPayload(aForPlayer, std::move(aEventName),
                                  [&](hg::util::Packet& aPacket) {
                                      count++;
                                      const auto payload = aPacket.extractOrThrow<taPayloadType>();
                                      aHandler(payload);
                                  });
        return count;
    }

private:
    InputSyncManagerInterface& _mgr;
    mutable hg::util::Packet _helperPacket;
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_INPUT_SYNC_MANAGER_INTERFACE_HPP