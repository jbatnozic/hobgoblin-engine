#include <Hobgoblin/Window/Window.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include <new>
#include <utility>

#include "Event_conversion.hpp"
#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace win {

namespace {
// Even for a regular Window, we must inherit from sf::RenderWindow,
// so that hg::RenderWindow would work properly with its inheritance
// structure.
class SfRenderWindowAdapter : public sf::RenderWindow {
public:
    template <class ...taArgs>
    SfRenderWindowAdapter(detail::WindowPolymorphismAdapter& aWindowPolyAdapter, taArgs&&... aArgs)
        : RenderWindow{std::forward<taArgs>(aArgs)...}
        , _windowPolyAdapter{aWindowPolyAdapter}
    {
    }

    void onCreate() override {
        _windowPolyAdapter.onCreate();
    }

    void onResize() override {
        _windowPolyAdapter.onResize();
    }

private:
    detail::WindowPolymorphismAdapter& _windowPolyAdapter;
};
} // namespace

class Window::Impl {
public:
    template <class ...taArgs>
    Impl(detail::WindowPolymorphismAdapter& aWindowPolyAdapter, taArgs&&... aArgs)
        : adapter{aWindowPolyAdapter, std::forward<taArgs>(aArgs)...}
    {
    }

    SfRenderWindowAdapter adapter;
};

#define  IMPLOF(_obj_) (&((_obj_)._impl->adapter))
#define CIMPLOF(_obj_) (&((_obj_)._impl->adapter))
#define  SELF_IMPL (IMPLOF(SELF))
#define SELF_CIMPL (CIMPLOF(SELF))

Window::Window()
    : _impl{std::make_unique<Impl>(static_cast<detail::WindowPolymorphismAdapter&>(SELF))}
{
}

Window::Window(VideoMode aMode, const std::string& aTitle, WindowStyle aStyle, const ContextSettings& aSettings)
    : _impl{std::make_unique<Impl>(static_cast<detail::WindowPolymorphismAdapter&>(SELF),
                                   ToSf(aMode),
                                   aTitle,
                                   ToSf(aStyle),
                                   ToSf(aSettings))}
{
}

Window::Window(WindowHandle aHandle, const ContextSettings& aSettings)
    : _impl{std::make_unique<Impl>(static_cast<detail::WindowPolymorphismAdapter&>(SELF),
                                   aHandle,
                                   ToSf(aSettings))}
{
}

Window::Window(Window&& aOther)
    : _impl{std::move(aOther._impl)}
{
}

Window& Window::operator=(Window&& aOther) {
    if (this != &aOther) {
        _impl = std::move(aOther._impl);
    }
    return SELF;
}

Window::~Window() = default;

void Window::create(VideoMode aMode, const std::string& aTitle, WindowStyle aStyle, const ContextSettings& aSettings) {
    SELF_IMPL->create(ToSf(aMode), aTitle, ToSf(aStyle), ToSf(aSettings));
}

void Window::create(WindowHandle aHandle, const ContextSettings& aSettings) {
    SELF_IMPL->create(aHandle, ToSf(aSettings));
}

void Window::close() {
    SELF_IMPL->close();
}

bool Window::isOpen() const {
    return SELF_CIMPL->isOpen();
}

ContextSettings Window::getSettings() const {
    return ToHg(SELF_CIMPL->getSettings());
}

bool Window::pollEvent(Event& aEvent) {
    sf::Event ev;
    const bool success = SELF_IMPL->pollEvent(ev);
    if (success) {
        aEvent = ToHg(ev);
    }
    return success;
}

bool Window::waitEvent(Event& aEvent) {
    sf::Event ev;
    const bool success = SELF_IMPL->waitEvent(ev);
    if (success) {
        aEvent = ToHg(ev);
    }
    return success;
}

math::Vector2i Window::getPosition() const {
    return ToHg(SELF_CIMPL->getPosition());
}

void Window::setPosition(const math::Vector2i& aPosition) {
    SELF_IMPL->setPosition(ToSf(aPosition));
}

math::Vector2pz Window::getSize() const {
    const auto size = SELF_CIMPL->getSize();
    return {
        static_cast<PZInteger>(size.x),
        static_cast<PZInteger>(size.y)
    };
}

void Window::setSize(const math::Vector2pz& aSize) {
    SELF_IMPL->setSize({static_cast<unsigned>(aSize.x), static_cast<unsigned>(aSize.y)});
}

void Window::setTitle(const std::string& aTitle) {
    SELF_IMPL->setTitle(aTitle);
}

void Window::setIcon(PZInteger aWidth, PZInteger aHeight, const std::uint8_t* aPixels) {
    SELF_IMPL->setIcon(static_cast<unsigned>(aWidth), static_cast<unsigned>(aHeight), aPixels);
}

void Window::setVisible(bool aVisible) {
    SELF_IMPL->setVisible(aVisible);
}

void Window::setVerticalSyncEnabled(bool aEnabled) {
    SELF_IMPL->setVerticalSyncEnabled(aEnabled);
}

void Window::setMouseCursorVisible(bool aVisible) {
    SELF_IMPL->setMouseCursorVisible(aVisible);
}

void Window::setMouseCursorGrabbed(bool aGrabbed) {
    SELF_IMPL->setMouseCursorGrabbed(aGrabbed);
}

void Window::setMouseCursor(const Cursor& aCursor) {
    // TODO
}

void Window::setKeyRepeatEnabled(bool aEnabled) {
    SELF_IMPL->setKeyRepeatEnabled(aEnabled);
}

void Window::setFramerateLimit(PZInteger aLimit) {
    SELF_IMPL->setFramerateLimit(static_cast<unsigned>(aLimit));
}

void Window::setJoystickThreshold(float aThreshold) {
    SELF_IMPL->setJoystickThreshold(aThreshold);
}

bool Window::setActive(bool aActive) {
    return SELF_IMPL->setActive(aActive);
}

void Window::requestFocus() {
    SELF_IMPL->requestFocus();
}

bool Window::hasFocus() const {
    return SELF_CIMPL->hasFocus();
}

void Window::display() {
    SELF_IMPL->display();
}

WindowHandle Window::getSystemHandle() const {
    return SELF_CIMPL->getSystemHandle();
}

void Window::onCreate() {
    // Nothing to do
}

void Window::onResize() {
    // Nothing to do
}

void* Window::_getSFMLImpl() {
    return static_cast<sf::RenderWindow*>(SELF_IMPL);
}

const void* Window::_getSFMLImpl() const {
    return static_cast<const sf::RenderWindow*>(SELF_CIMPL);
}

} // namespace win
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
