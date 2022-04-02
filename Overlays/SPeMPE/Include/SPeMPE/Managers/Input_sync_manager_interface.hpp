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
    virtual void applyNewInput() = 0; // pre step
    virtual void advance() = 0; // post step

    virtual void sendInput(hg::RN_NodeInterface& aNode) = 0;

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
    void defineSignal(std::string aSignalName, const taSignalType& aInitialValue) {
        _helperPacket.clear();
        _helperPacket << aInitialValue;
        _mgr.defineSignal(std::move(aSignalName), typeid(taSignalType), _helperPacket);
        _helperPacket.clear();
    }

    void defineSimpleEvent(std::string aEventName) {
        _mgr.defineSimpleEvent(std::move(aEventName));
    }

    template <class taSignalType>
    void setSignalValue(std::string aSignalName, const taSignalType& aValue) {
        if (typeid(taSignalType) != _mgr.getSignalType(aSignalName)) {
            throw hg::TracedLogicError{"InputSyncManagerAdapter - Incorrect type for signal value!"};
        }
        _mgr.setSignalValue(std::move(aSignalName),
                            [&](hg::util::Packet& aPacket) {
                                aPacket << aValue;
                            });
    }

    template <class taSignalType>
    void setSignalValue(hg::PZInteger aForPlayer, std::string aSignalName, const taSignalType& aValue) {
        if (typeid(taSignalType) != _mgr.getSignalType(aSignalName)) {
            throw hg::TracedLogicError{"InputSyncManagerAdapter - Incorrect type for signal value!"};
        }
        _mgr.setSignalValue(aForPlayer,
                            std::move(aSignalName),
                            [&](hg::util::Packet& aPacket) {
                                aPacket << aValue;
                            });
    }

    void triggerEvent(std::string aEventName, bool aPredicate = true) {
        if (aPredicate) {
            _mgr.triggerEvent(std::move(aEventName));
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

    void pollSimpleEvent(hg::PZInteger aForPlayer,
                         std::string aEventName,
                         const std::function<void()>& aHandler) const {
        _mgr.pollSimpleEvent(aForPlayer, std::move(aEventName), aHandler);
    }

private:
    InputSyncManagerInterface& _mgr;
    mutable hg::util::Packet _helperPacket;
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_INPUT_SYNC_MANAGER_INTERFACE_HPP