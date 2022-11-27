#ifndef SPEMPE_MANAGERS_INPUT_SYNC_MANAGER_INTERFACE_HPP
#define SPEMPE_MANAGERS_INPUT_SYNC_MANAGER_INTERFACE_HPP

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
    ~InputSyncManagerInterface() override = default;

    //! Initializes the manager as the host for up to 'aClientCount' clients. Note: if 'aClientCount'
    //! is 0, the manager will be able only to echo the inputs of the local player.
    virtual void setToHostMode(hg::PZInteger aClientCount, hg::PZInteger aStateBufferingLength) = 0;

    virtual void setToClientMode() = 0;

    virtual void setStateBufferingLength(hg::PZInteger aNewStateBufferingLength) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // INPUT DEFINITIONS                                                     //
    ///////////////////////////////////////////////////////////////////////////
    
    // Note: Use the InputSyncManagerWrapper to manage signal & event definitions

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

    // Note: Use the InputSyncManagerWrapper to set signals & events

    virtual void setSignalValue(std::string aSignalName,
                                const std::function<void(hg::util::Packet&)>& f) = 0;
    
    virtual void triggerEvent(std::string aEventName) = 0;
    
    virtual void triggerEventWithPayload(std::string aEventName,
                                         const std::function<void(hg::util::Packet&)>& f) = 0;
    
    ///////////////////////////////////////////////////////////////////////////
    // SETTING INPUT VALUES (SERVER-SIDE)                                    //
    ///////////////////////////////////////////////////////////////////////////

    // Note: Use the InputSyncManagerWrapper to set signals & events

    virtual void setSignalValue(int aForClient,
                                std::string aSignalName,
                                const std::function<void(hg::util::Packet&)>& f) = 0;

    virtual void triggerEvent(int aForClient, std::string aEventName) = 0;

    virtual void triggerEventWithPayload(int aForClient, 
                                         std::string aEventName,
                                         const std::function<void(hg::util::Packet&)>& f) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // GETTING INPUT VALUES (SERVER-SIDE)                                    //
    ///////////////////////////////////////////////////////////////////////////

    // Note: Use the InputSyncManagerWrapper to get signal & event values

    virtual void getSignalValue(int aForClient,
                                std::string aSignalName,
                                hg::util::Packet& aPacket) const = 0;

    virtual void pollSimpleEvent(int aForClient,
                                 std::string aEventName,
                                 const std::function<void()>& aHandler) const = 0;

    virtual void pollEventWithPayload(int aForClient,
                                      std::string aEventName,
                                      const std::function<void(hg::util::Packet&)>& aPayloadHandler) const = 0;

private:
    SPEMPE_CTXCOMP_TAG("jbatnozic::spempe::InputSyncManager");
};

//! The bare InputSyncManagerInterface has very unwieldy methods which are difficult to use, so
//! in any place where you want to define, set or get inputs, you can construct an instance of
//! 'InputSyncManagerWrapper' instead and use its templated methods which are must more ergonomic.
//! This wrapper is very lightweight to construct so there isn't much overhead (if any) when using
//! it, and you don't have to keep the instance around - just construct a new one when needed.
class InputSyncManagerWrapper {
public:
    InputSyncManagerWrapper(InputSyncManagerInterface& aMgr)
        : _mgr{aMgr}
    {
    }

    ///////////////////////////////////////////////////////////////////////////
    // INPUT DEFINITIONS                                                     //
    ///////////////////////////////////////////////////////////////////////////

    //! Defines a signal of type 'taSignalType', with the name 'aSignalName', and with the initial
    //! value of 'aInitialValue'.
    template <class taSignalType>
    void defineSignal(std::string aSignalName, const taSignalType& aInitialValue) const {
        _helperPacket.clear();
        _helperPacket << aInitialValue;
        _mgr.defineSignal(std::move(aSignalName), typeid(taSignalType), _helperPacket);
        _helperPacket.clear();
    }

    //! Defines a simple event with the name 'aEventName'.
    void defineSimpleEvent(std::string aEventName) const {
        _mgr.defineSimpleEvent(std::move(aEventName));
    }

    //! Defines an event with a payload (of type 'taPayloadType') with the name 'aEventName'.
    template <class taPayloadType>
    void defineEventWithPayload(std::string aEventName) const {
        _mgr.defineEventWithPayload(std::move(aEventName), typeid(taPayloadType));
    }

    ///////////////////////////////////////////////////////////////////////////
    // SETTING INPUT VALUES (CLIENT-SIDE)                                    //
    ///////////////////////////////////////////////////////////////////////////

    //! Sets the signal with the name 'aSignalName' to the value 'aValue'.
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

    //! Triggers the signal with the name 'aEventName' if 'aPredicate' is true.
    void triggerEvent(std::string aEventName, bool aPredicate = true) const {
        if (aPredicate) {
            _mgr.triggerEvent(std::move(aEventName));
        }
    }

    //! Triggers the signal with the name 'aEventName' if 'aPredicate' is true, and attaches the
    //! payload (of type 'taPayloadType') 'aPayload'.
    template <class taPayloadType>
    void triggerEventWithPayload(std::string aEventName,
                                 const taPayloadType& aPayload,
                                 bool aPredicate = true) const {
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

    ///////////////////////////////////////////////////////////////////////////
    // SETTING INPUT VALUES (SERVER-SIDE)                                    //
    ///////////////////////////////////////////////////////////////////////////

    //! Sets the signal with the name 'aSignalName' to the value 'aValue' for client with index
    //! 'aForClient' (or for the host if spempe::CLIENT_INDEX_LOCAL is provided).
    template <class taSignalType>
    void setSignalValue(int aForClient, std::string aSignalName, const taSignalType& aValue) const {
        if (typeid(taSignalType) != _mgr.getSignalType(aSignalName)) {
            throw hg::TracedLogicError{"InputSyncManagerAdapter - Incorrect type for signal value!"};
        }
        _mgr.setSignalValue(aForClient,
                            std::move(aSignalName),
                            [&](hg::util::Packet& aPacket) {
            aPacket << aValue;
        });
    }

    //! Triggers the signal with the name 'aEventName' if 'aPredicate' is true, for client with
    //! index 'aForClient' (or for the host if spempe::CLIENT_INDEX_LOCAL is provided).
    void triggerEvent(int aForClient, std::string aEventName, bool aPredicate = true) const {
        if (aPredicate) {
            _mgr.triggerEvent(aForClient, std::move(aEventName));
        }
    }

    //! Triggers the signal with the name 'aEventName' if 'aPredicate' is true, and attaches the
    //! payload (of type 'taPayloadType') 'aPayload', for client with index 'aForClient'
    //! (or for the host if spempe::CLIENT_INDEX_LOCAL is provided).
    template <class taPayloadType>
    void triggerEventWithPayload(int aForClient,
                                 std::string aEventName,
                                 const taPayloadType& aPayload,
                                 bool aPredicate = true) const {
        if (typeid(taPayloadType) != _mgr.getEventPayloadType(aEventName)) {
            throw hg::TracedLogicError{"InputSyncManagerAdapter - Incorrect type for payload!"};
        }

        if (aPredicate) {
            _mgr.triggerEventWithPayload(aForClient,
                                         std::move(aEventName),
                                         [&](hg::util::Packet& aPacket) {
                aPacket << aPayload;
            });
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // GETTING INPUT VALUES (SERVER-SIDE)                                    //
    ///////////////////////////////////////////////////////////////////////////

    //! TODO (add description)
    template <class taSignalType>
    taSignalType getSignalValue(int aForClient,
                                std::string aSignalName) const {
        if (typeid(taSignalType) != _mgr.getSignalType(aSignalName)) {
            throw hg::TracedLogicError{"InputSyncManagerAdapter - Incorrect type for signal value!"};
        }
        _helperPacket.clear();
        _mgr.getSignalValue(aForClient, std::move(aSignalName), _helperPacket);
        taSignalType retval;
        _helperPacket >> retval;
        _helperPacket.clear();
        return retval;
    }

    //! TODO (add description)
    hg::PZInteger pollSimpleEvent(int aForClient,
                                  std::string aEventName,
                                  const std::function<void()>& aHandler) const {
        hg::PZInteger count = 0;
        _mgr.pollSimpleEvent(aForClient, std::move(aEventName),
                             [&]() {
                                 count++;
                                 aHandler();
                             });
        return count;
    }

    //! TODO (add description)
    hg::PZInteger countSimpleEvent(int aForClient,
                          std::string aEventName) const {
        hg::PZInteger count = 0;
        _mgr.pollSimpleEvent(aForClient, 
                             std::move(aEventName), 
                             [&]() {
                                 count++;
                             });
        return count;
    }

    //! TODO (add description)
    template <class taPayloadType>
    hg::PZInteger pollEventWithPayload(int aForClient,
                                       std::string aEventName,
                                       const std::function<void(const taPayloadType&)>& aHandler) const {
        if (typeid(taPayloadType) != _mgr.getEventPayloadType(aEventName)) {
            throw hg::TracedLogicError{"InputSyncManagerAdapter - Incorrect type for payload!"};
        }

        hg::PZInteger count = 0;
        _mgr.pollEventWithPayload(aForClient, std::move(aEventName),
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