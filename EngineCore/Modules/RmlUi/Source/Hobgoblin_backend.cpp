
#include <Hobgoblin/RmlUi/Hobgoblin_backend.hpp>

#include <RmlUi/Core.h>

#include <Hobgoblin/RmlUi/Private/RmlUi_SFML_renderer.hpp>
#include <Hobgoblin/RmlUi/Private/RmlUi_SFML_system.hpp>

#include <mutex>
#include <stdexcept>

#include <Hobgoblin/Private/Pmacro_define.hpp>
HOBGOBLIN_NAMESPACE_BEGIN
namespace rml {

namespace {
std::mutex gBackendMutex;
int gBackendUseCount;

std::unique_ptr<detail::RmlUiSFMLSystem>   gSystem;
std::unique_ptr<detail::RmlUiSFMLRenderer> gRenderer;
} // namespace

std::unique_ptr<HobgoblinBackend::BackendLifecycleGuard> HobgoblinBackend::initialize() {
    std::lock_guard lock{gBackendMutex};

    if (gBackendUseCount == 0) {
        gSystem = std::make_unique<detail::RmlUiSFMLSystem>();
        gRenderer = std::make_unique<detail::RmlUiSFMLRenderer>();

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

Rml::Input::KeyIdentifier HobgoblinBackend::translateKey(sf::Keyboard::Key aKey) {
    switch (aKey) {
    case sf::Keyboard::A:
        return Rml::Input::KI_A;

    case sf::Keyboard::B:
        return Rml::Input::KI_B;

    case sf::Keyboard::C:
        return Rml::Input::KI_C;

    case sf::Keyboard::D:
        return Rml::Input::KI_D;

    case sf::Keyboard::E:
        return Rml::Input::KI_E;

    case sf::Keyboard::F:
        return Rml::Input::KI_F;

    case sf::Keyboard::G:
        return Rml::Input::KI_G;

    case sf::Keyboard::H:
        return Rml::Input::KI_H;

    case sf::Keyboard::I:
        return Rml::Input::KI_I;

    case sf::Keyboard::J:
        return Rml::Input::KI_J;

    case sf::Keyboard::K:
        return Rml::Input::KI_K;

    case sf::Keyboard::L:
        return Rml::Input::KI_L;

    case sf::Keyboard::M:
        return Rml::Input::KI_M;

    case sf::Keyboard::N:
        return Rml::Input::KI_N;

    case sf::Keyboard::O:
        return Rml::Input::KI_O;

    case sf::Keyboard::P:
        return Rml::Input::KI_P;

    case sf::Keyboard::Q:
        return Rml::Input::KI_Q;

    case sf::Keyboard::R:
        return Rml::Input::KI_R;

    case sf::Keyboard::S:
        return Rml::Input::KI_S;

    case sf::Keyboard::T:
        return Rml::Input::KI_T;

    case sf::Keyboard::U:
        return Rml::Input::KI_U;

    case sf::Keyboard::V:
        return Rml::Input::KI_V;

    case sf::Keyboard::W:
        return Rml::Input::KI_W;

    case sf::Keyboard::X:
        return Rml::Input::KI_X;

    case sf::Keyboard::Y:
        return Rml::Input::KI_Y;

    case sf::Keyboard::Z:
        return Rml::Input::KI_Z;

    case sf::Keyboard::Num0:
        return Rml::Input::KI_0;

    case sf::Keyboard::Num1:
        return Rml::Input::KI_1;

    case sf::Keyboard::Num2:
        return Rml::Input::KI_2;

    case sf::Keyboard::Num3:
        return Rml::Input::KI_3;

    case sf::Keyboard::Num4:
        return Rml::Input::KI_4;

    case sf::Keyboard::Num5:
        return Rml::Input::KI_5;

    case sf::Keyboard::Num6:
        return Rml::Input::KI_6;

    case sf::Keyboard::Num7:
        return Rml::Input::KI_7;

    case sf::Keyboard::Num8:
        return Rml::Input::KI_8;

    case sf::Keyboard::Num9:
        return Rml::Input::KI_9;

    case sf::Keyboard::Numpad0:
        return Rml::Input::KI_NUMPAD0;

    case sf::Keyboard::Numpad1:
        return Rml::Input::KI_NUMPAD1;

    case sf::Keyboard::Numpad2:
        return Rml::Input::KI_NUMPAD2;

    case sf::Keyboard::Numpad3:
        return Rml::Input::KI_NUMPAD3;

    case sf::Keyboard::Numpad4:
        return Rml::Input::KI_NUMPAD4;

    case sf::Keyboard::Numpad5:
        return Rml::Input::KI_NUMPAD5;

    case sf::Keyboard::Numpad6:
        return Rml::Input::KI_NUMPAD6;

    case sf::Keyboard::Numpad7:
        return Rml::Input::KI_NUMPAD7;

    case sf::Keyboard::Numpad8:
        return Rml::Input::KI_NUMPAD8;

    case sf::Keyboard::Numpad9:
        return Rml::Input::KI_NUMPAD9;

    case sf::Keyboard::Left:
        return Rml::Input::KI_LEFT;

    case sf::Keyboard::Right:
        return Rml::Input::KI_RIGHT;

    case sf::Keyboard::Up:
        return Rml::Input::KI_UP;

    case sf::Keyboard::Down:
        return Rml::Input::KI_DOWN;

    case sf::Keyboard::Add:
        return Rml::Input::KI_ADD;

    case sf::Keyboard::BackSpace:
        return Rml::Input::KI_BACK;

    case sf::Keyboard::Delete:
        return Rml::Input::KI_DELETE;

    case sf::Keyboard::Divide:
        return Rml::Input::KI_DIVIDE;

    case sf::Keyboard::End:
        return Rml::Input::KI_END;

    case sf::Keyboard::Escape:
        return Rml::Input::KI_ESCAPE;

    case sf::Keyboard::F1:
        return Rml::Input::KI_F1;

    case sf::Keyboard::F2:
        return Rml::Input::KI_F2;

    case sf::Keyboard::F3:
        return Rml::Input::KI_F3;

    case sf::Keyboard::F4:
        return Rml::Input::KI_F4;

    case sf::Keyboard::F5:
        return Rml::Input::KI_F5;

    case sf::Keyboard::F6:
        return Rml::Input::KI_F6;

    case sf::Keyboard::F7:
        return Rml::Input::KI_F7;

    case sf::Keyboard::F8:
        return Rml::Input::KI_F8;

    case sf::Keyboard::F9:
        return Rml::Input::KI_F9;

    case sf::Keyboard::F10:
        return Rml::Input::KI_F10;

    case sf::Keyboard::F11:
        return Rml::Input::KI_F11;

    case sf::Keyboard::F12:
        return Rml::Input::KI_F12;

    case sf::Keyboard::F13:
        return Rml::Input::KI_F13;

    case sf::Keyboard::F14:
        return Rml::Input::KI_F14;

    case sf::Keyboard::F15:
        return Rml::Input::KI_F15;

    case sf::Keyboard::Home:
        return Rml::Input::KI_HOME;

    case sf::Keyboard::Insert:
        return Rml::Input::KI_INSERT;

    case sf::Keyboard::LControl:
        return Rml::Input::KI_LCONTROL;

    case sf::Keyboard::LShift:
        return Rml::Input::KI_LSHIFT;

    case sf::Keyboard::Multiply:
        return Rml::Input::KI_MULTIPLY;

    case sf::Keyboard::Pause:
        return Rml::Input::KI_PAUSE;

    case sf::Keyboard::RControl:
        return Rml::Input::KI_RCONTROL;

    case sf::Keyboard::Return:
        return Rml::Input::KI_RETURN;

    case sf::Keyboard::RShift:
        return Rml::Input::KI_RSHIFT;

    case sf::Keyboard::Space:
        return Rml::Input::KI_SPACE;

    case sf::Keyboard::Subtract:
        return Rml::Input::KI_SUBTRACT;

    case sf::Keyboard::Tab:
        return Rml::Input::KI_TAB;

    default:
        break;
    };

    return Rml::Input::KI_UNKNOWN;
}

int HobgoblinBackend::getKeyModifiers() {
    int modifiers = 0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
        modifiers |= Rml::Input::KM_SHIFT;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
        modifiers |= Rml::Input::KM_CTRL;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt)) {
        modifiers |= Rml::Input::KM_ALT;
    }

    return modifiers; // TODO There are other modifiers (such as capslock)
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE                                                               //
///////////////////////////////////////////////////////////////////////////

detail::RmlUiSFMLSystem* HobgoblinBackend::getSystem() {
    std::lock_guard lock{gBackendMutex};
    return gSystem.get();
}

detail::RmlUiSFMLRenderer* HobgoblinBackend::getRenderer() {
    std::lock_guard lock{gBackendMutex};
    return gRenderer.get();
}

} // namespace rml
HOBGOBLIN_NAMESPACE_END
#include <Hobgoblin/Private/Pmacro_undef.hpp>
