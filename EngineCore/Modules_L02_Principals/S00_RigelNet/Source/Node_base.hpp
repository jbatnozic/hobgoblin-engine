// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_RN_NODE_BASE_HPP
#define UHOBGOBLIN_RN_NODE_BASE_HPP

#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/RigelNet/Events.hpp>
#include <Hobgoblin/Utility/Any_ptr.hpp>

#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

namespace {
constexpr auto LOG_ID = "Hobgoblin.RigelNet";
}

class RN_NodeBase {
protected:
    std::vector<RN_EventListener*> _eventListeners;
    util::AnyPtr                   _userData;

    void _addEventListener(NeverNull<RN_EventListener*> aEventListener) {
        const auto iter = std::find_if(_eventListeners.begin(),
                                       _eventListeners.end(),
                                       [=](RN_EventListener* aFoundListener) {
                                           return (aFoundListener == aEventListener);
                                       });
        if (iter == _eventListeners.end()) {
            _eventListeners.push_back(aEventListener);
        } else {
            HG_LOG_WARN(LOG_ID,
                        "Listener matching pointer {:#x} was already added.",
                        reinterpret_cast<std::uintptr_t>(aEventListener.get()));
        }
    }

    void _removeEventListener(NeverNull<RN_EventListener*> aEventListener) {
        const auto iter = std::find_if(_eventListeners.begin(),
                                       _eventListeners.end(),
                                       [=](RN_EventListener* aFoundListener) {
                                           return (aFoundListener == aEventListener);
                                       });
        if (iter != _eventListeners.end()) {
            _eventListeners.erase(iter);
        } else {
            HG_LOG_WARN(LOG_ID,
                        "No listener matching pointer {:#x} was found to remove.",
                        reinterpret_cast<std::uintptr_t>(aEventListener.get()));
        }
    }
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_NODE_BASE_HPP
