// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/RmlUi/Hobgoblin_backend.hpp>

#include <RmlUi/Core.h>

#include <Hobgoblin/RmlUi/Private/RmlUi_Hobgoblin_renderer.hpp>
#include <Hobgoblin/RmlUi/Private/RmlUi_Hobgoblin_system.hpp>

#include <mutex>
#include <stdexcept>

#include <Hobgoblin/Private/Pmacro_define.hpp>
HOBGOBLIN_NAMESPACE_BEGIN
namespace rml {

namespace {
std::mutex gBackendMutex;
int gBackendUseCount;

std::unique_ptr<detail::RmlUiHobgoblinSystem>   gSystem;
std::unique_ptr<detail::RmlUiHobgoblinRenderer> gRenderer;
} // namespace

std::unique_ptr<HobgoblinBackend::BackendLifecycleGuard> HobgoblinBackend::initialize() {
    std::lock_guard lock{gBackendMutex};

    if (gBackendUseCount == 0) {
        gSystem = std::make_unique<detail::RmlUiHobgoblinSystem>();
        gRenderer = std::make_unique<detail::RmlUiHobgoblinRenderer>();

        Rml::SetSystemInterface(gSystem.get());
        Rml::SetRenderInterface(gRenderer.get());

        if (!Rml::Initialise()) {
            throw std::runtime_error{"HobgoblinBackend - RmlUi could not be initialized!"};
        }
    }

    gBackendUseCount += 1;
    return std::make_unique<BackendLifecycleGuard>();
}

HobgoblinBackend::BackendLifecycleGuard::~BackendLifecycleGuard() {
    std::lock_guard lock{gBackendMutex};

    gBackendUseCount -= 1;

    if (gBackendUseCount == 0) {
        Rml::Shutdown();

        gRenderer.reset();
        gSystem.reset();
    }
}

Rml::Input::KeyIdentifier HobgoblinBackend::translateKey(in::VirtualKeyboardKey aKey) {
    const in::UniversalInputEnum e = aKey;
    switch (e) {
    case in::VK_A:
        return Rml::Input::KI_A;

    case in::VK_B:
        return Rml::Input::KI_B;

    case in::VK_C:
        return Rml::Input::KI_C;

    case in::VK_D:
        return Rml::Input::KI_D;

    case in::VK_E:
        return Rml::Input::KI_E;

    case in::VK_F:
        return Rml::Input::KI_F;

    case in::VK_G:
        return Rml::Input::KI_G;

    case in::VK_H:
        return Rml::Input::KI_H;

    case in::VK_I:
        return Rml::Input::KI_I;

    case in::VK_J:
        return Rml::Input::KI_J;

    case in::VK_K:
        return Rml::Input::KI_K;

    case in::VK_L:
        return Rml::Input::KI_L;

    case in::VK_M:
        return Rml::Input::KI_M;

    case in::VK_N:
        return Rml::Input::KI_N;

    case in::VK_O:
        return Rml::Input::KI_O;

    case in::VK_P:
        return Rml::Input::KI_P;

    case in::VK_Q:
        return Rml::Input::KI_Q;

    case in::VK_R:
        return Rml::Input::KI_R;

    case in::VK_S:
        return Rml::Input::KI_S;

    case in::VK_T:
        return Rml::Input::KI_T;

    case in::VK_U:
        return Rml::Input::KI_U;

    case in::VK_V:
        return Rml::Input::KI_V;

    case in::VK_W:
        return Rml::Input::KI_W;

    case in::VK_X:
        return Rml::Input::KI_X;

    case in::VK_Y:
        return Rml::Input::KI_Y;

    case in::VK_Z:
        return Rml::Input::KI_Z;

    case in::VK_NUM_0:
        return Rml::Input::KI_0;

    case in::VK_NUM_1:
        return Rml::Input::KI_1;

    case in::VK_NUM_2:
        return Rml::Input::KI_2;

    case in::VK_NUM_3:
        return Rml::Input::KI_3;

    case in::VK_NUM_4:
        return Rml::Input::KI_4;

    case in::VK_NUM_5:
        return Rml::Input::KI_5;

    case in::VK_NUM_6:
        return Rml::Input::KI_6;

    case in::VK_NUM_7:
        return Rml::Input::KI_7;

    case in::VK_NUM_8:
        return Rml::Input::KI_8;

    case in::VK_NUM_9:
        return Rml::Input::KI_9;

    case in::VK_NUMPAD_0:
        return Rml::Input::KI_NUMPAD0;

    case in::VK_NUMPAD_1:
        return Rml::Input::KI_NUMPAD1;

    case in::VK_NUMPAD_2:
        return Rml::Input::KI_NUMPAD2;

    case in::VK_NUMPAD_3:
        return Rml::Input::KI_NUMPAD3;

    case in::VK_NUMPAD_4:
        return Rml::Input::KI_NUMPAD4;

    case in::VK_NUMPAD_5:
        return Rml::Input::KI_NUMPAD5;

    case in::VK_NUMPAD_6:
        return Rml::Input::KI_NUMPAD6;

    case in::VK_NUMPAD_7:
        return Rml::Input::KI_NUMPAD7;

    case in::VK_NUMPAD_8:
        return Rml::Input::KI_NUMPAD8;

    case in::VK_NUMPAD_9:
        return Rml::Input::KI_NUMPAD9;

    case in::VK_LEFT:
        return Rml::Input::KI_LEFT;

    case in::VK_RIGHT:
        return Rml::Input::KI_RIGHT;

    case in::VK_UP:
        return Rml::Input::KI_UP;

    case in::VK_DOWN:
        return Rml::Input::KI_DOWN;

    case in::VK_ADD:
        return Rml::Input::KI_ADD;

    case in::VK_BACKSPACE:
        return Rml::Input::KI_BACK;

    case in::VK_DELETE:
        return Rml::Input::KI_DELETE;

    case in::VK_DIVIDE:
        return Rml::Input::KI_DIVIDE;

    case in::VK_END:
        return Rml::Input::KI_END;

    case in::VK_ESCAPE:
        return Rml::Input::KI_ESCAPE;

    case in::VK_F1:
        return Rml::Input::KI_F1;

    case in::VK_F2:
        return Rml::Input::KI_F2;

    case in::VK_F3:
        return Rml::Input::KI_F3;

    case in::VK_F4:
        return Rml::Input::KI_F4;

    case in::VK_F5:
        return Rml::Input::KI_F5;

    case in::VK_F6:
        return Rml::Input::KI_F6;

    case in::VK_F7:
        return Rml::Input::KI_F7;

    case in::VK_F8:
        return Rml::Input::KI_F8;

    case in::VK_F9:
        return Rml::Input::KI_F9;

    case in::VK_F10:
        return Rml::Input::KI_F10;

    case in::VK_F11:
        return Rml::Input::KI_F11;

    case in::VK_F12:
        return Rml::Input::KI_F12;

    case in::VK_F13:
        return Rml::Input::KI_F13;

    case in::VK_F14:
        return Rml::Input::KI_F14;

    case in::VK_F15:
        return Rml::Input::KI_F15;

    case in::VK_HOME:
        return Rml::Input::KI_HOME;

    case in::VK_INSERT:
        return Rml::Input::KI_INSERT;

    case in::VK_LCONTROL:
        return Rml::Input::KI_LCONTROL;

    case in::VK_LSHIFT:
        return Rml::Input::KI_LSHIFT;

    case in::VK_MULTIPLY:
        return Rml::Input::KI_MULTIPLY;

    case in::VK_PAUSE:
        return Rml::Input::KI_PAUSE;

    case in::VK_RCONTROL:
        return Rml::Input::KI_RCONTROL;

    case in::VK_ENTER:
        return Rml::Input::KI_RETURN;

    case in::VK_RSHIFT:
        return Rml::Input::KI_RSHIFT;

    case in::VK_SPACE:
        return Rml::Input::KI_SPACE;

    case in::VK_SUBTRACT:
        return Rml::Input::KI_SUBTRACT;

    case in::VK_TAB:
        return Rml::Input::KI_TAB;

    default:
        break;
    };

    return Rml::Input::KI_UNKNOWN;
}

int HobgoblinBackend::translateButton(in::MouseButton aButton) {
    return static_cast<int>(aButton.val()) - static_cast<int>(HG_INPUT_MOUSEBUTTON_FIRST);
}

int HobgoblinBackend::getKeyModifiers() {
    int modifiers = 0;

    if (in::CheckPressedVK(in::VK_LSHIFT) ||
        in::CheckPressedVK(in::VK_RSHIFT)) {
        modifiers |= Rml::Input::KM_SHIFT;
    }

    if (in::CheckPressedVK(in::VK_LCONTROL) ||
        in::CheckPressedVK(in::VK_RCONTROL)) {
        modifiers |= Rml::Input::KM_CTRL;
    }

    if (in::CheckPressedVK(in::VK_LALT) ||
        in::CheckPressedVK(in::VK_RALT)) {
        modifiers |= Rml::Input::KM_ALT;
    }

    return modifiers; // TODO There are other modifiers (such as capslock)
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE                                                               //
///////////////////////////////////////////////////////////////////////////

detail::RmlUiHobgoblinSystem* HobgoblinBackend::getSystem() {
    std::lock_guard lock{gBackendMutex};
    return gSystem.get();
}

detail::RmlUiHobgoblinRenderer* HobgoblinBackend::getRenderer() {
    std::lock_guard lock{gBackendMutex};
    return gRenderer.get();
}

} // namespace rml
HOBGOBLIN_NAMESPACE_END
#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
