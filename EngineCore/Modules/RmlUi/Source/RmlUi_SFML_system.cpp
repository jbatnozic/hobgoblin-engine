
#include <Hobgoblin/RmlUi/Private/RmlUi_SFML_system.hpp>

#include <Hobgoblin/Logging.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>
HOBGOBLIN_NAMESPACE_BEGIN
namespace rml {
namespace detail {

namespace {
constexpr auto LOG_ID = "RmlUiSFMLSystem";
} // namespace

RmlUiSFMLSystem::~RmlUiSFMLSystem() = default;

double RmlUiSFMLSystem::GetElapsedTime() {
    return _clock.getElapsedTime().asSeconds();
}

bool RmlUiSFMLSystem::LogMessage(Rml::Log::Type aType, const Rml::String& aMessage) {
    switch (aType) {
        case Rml::Log::LT_ERROR:
            HG_LOG_ERROR(LOG_ID, "{}", aMessage);
            break;

        case Rml::Log::LT_ASSERT:
            HG_LOG_FATAL(LOG_ID, "{}", aMessage);
            break;
            
        case Rml::Log::LT_WARNING:
            HG_LOG_WARN(LOG_ID, "{}", aMessage);
            break;

        case Rml::Log::LT_INFO:
            HG_LOG_INFO(LOG_ID, "{}", aMessage);
            break;

        case Rml::Log::LT_DEBUG:
            HG_LOG_DEBUG(LOG_ID, "{}", aMessage);
            break;

        case Rml::Log::LT_ALWAYS:
        case Rml::Log::LT_MAX:
        default:
            HG_LOG_ERROR(LOG_ID, "{}", aMessage);
            break;
    }

    return true;
}

void RmlUiSFMLSystem::SetClipboardText(const Rml::String& aText) {
    sf::Clipboard::setString(
        sf::String::fromUtf8(aText.begin(), aText.end())
    );
}

void RmlUiSFMLSystem::GetClipboardText(Rml::String& aText) {
    aText = sf::Clipboard::getString().toAnsiString(); // TODO should I use toUtf8 instead?
}

} // namespace detail
} // namespace rml
HOBGOBLIN_NAMESPACE_END
#include <Hobgoblin/Private/Pmacro_undef.hpp>