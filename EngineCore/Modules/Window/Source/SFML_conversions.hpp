#ifndef HOBGOBLIN_GRAPHICS_SFML_CONVERSIONS_HPP
#define HOBGOBLIN_GRAPHICS_SFML_CONVERSIONS_HPP

// Hobgoblin

#include <Hobgoblin/Math/Vector.hpp>
#include <Hobgoblin/Window/Context_settings.hpp>
#include <Hobgoblin/Window/Cursor.hpp>
#include <Hobgoblin/Window/Video_mode.hpp>
#include <Hobgoblin/Window/Window_style.hpp>

// SFML

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/ContextSettings.hpp>
#include <SFML/Window/Cursor.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowStyle.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace win {

namespace detail {
class WindowImplAccessor {
public:
    template <class taSFMLImpl, class taGraphicsClass>
    static taSFMLImpl& getImplOf(taGraphicsClass& aGraphicsObject) {
        return *static_cast<taSFMLImpl*>(aGraphicsObject._getSFMLImpl());
    }
    template <class taSFMLImpl, class taGraphicsClass>
    static const taSFMLImpl& getImplOf(const taGraphicsClass& aGraphicsObject) {
        return *static_cast<const taSFMLImpl*>(aGraphicsObject._getSFMLImpl());
    }
};
} // namespace detail

///////////////////////////////////////////////////////////////////////////
// MATH                                                                  //
///////////////////////////////////////////////////////////////////////////

template <class taArithmetic>
typename sf::Vector2<taArithmetic> ToSf(const typename math::Vector2<taArithmetic>& aVector) {
    return {aVector.x, aVector.y};
}

template <class taArithmetic>
typename math::Vector2<taArithmetic> ToHg(const typename sf::Vector2<taArithmetic>& aVector) {
    return {aVector.x, aVector.y};
}

///////////////////////////////////////////////////////////////////////////
// DECLARATIONS                                                          //
///////////////////////////////////////////////////////////////////////////

// ContextSettings

ContextSettings::Attribute ToHg(sf::ContextSettings::Attribute aAttribute);
unsigned ToSf(ContextSettings::Attribute aAttribute);

ContextSettings ToHg(const sf::ContextSettings& aSettings);
sf::ContextSettings ToSf(const ContextSettings& aSettings);

// Cursor

sf::Cursor::Type ToSf(Cursor::Type aType);

// VideoMode

VideoMode ToHg(const sf::VideoMode& aVideoMode);
sf::VideoMode ToSf(const VideoMode& aVideoMode);

// WindowStyle

int ToSf(WindowStyle aStyle);

///////////////////////////////////////////////////////////////////////////
// INLINE DEFINITIONS                                                    //
///////////////////////////////////////////////////////////////////////////

// ContextSettings

inline
ContextSettings::Attribute ToHg(sf::ContextSettings::Attribute aAttribute) {
    if (aAttribute == sf::ContextSettings::Default) {
        return ContextSettings::Attribute::Default;
    }

    auto result = static_cast<ContextSettings::Attribute>(0);

    if ((aAttribute & sf::ContextSettings::Core)  != 0) result = result | ContextSettings::Attribute::Core;
    if ((aAttribute & sf::ContextSettings::Debug) != 0) result = result | ContextSettings::Attribute::Debug;

    return result;
}

inline
unsigned ToSf(ContextSettings::Attribute aAttribute) {
    if (aAttribute == ContextSettings::Attribute::Default) {
        return sf::ContextSettings::Default;
    }

    unsigned result = 0;

    if ((aAttribute & ContextSettings::Attribute::Core)  != static_cast<ContextSettings::Attribute>(0)) result |= sf::ContextSettings::Core;
    if ((aAttribute & ContextSettings::Attribute::Debug) != static_cast<ContextSettings::Attribute>(0)) result |= sf::ContextSettings::Debug;

    return result;
}

inline
ContextSettings ToHg(const sf::ContextSettings& aSettings) {
    return ContextSettings{
        static_cast<PZInteger>(aSettings.depthBits),
        static_cast<PZInteger>(aSettings.stencilBits),
        static_cast<PZInteger>(aSettings.antialiasingLevel),
        static_cast<PZInteger>(aSettings.majorVersion),
        static_cast<PZInteger>(aSettings.minorVersion),
        ToHg(static_cast<sf::ContextSettings::Attribute>(aSettings.attributeFlags)),
        aSettings.sRgbCapable
    };
}

inline
sf::ContextSettings ToSf(const ContextSettings& aSettings) {
    return sf::ContextSettings{
        static_cast<unsigned>(aSettings.depthBits),
        static_cast<unsigned>(aSettings.stencilBits),
        static_cast<unsigned>(aSettings.antialiasingLevel),
        static_cast<unsigned>(aSettings.majorVersion),
        static_cast<unsigned>(aSettings.minorVersion),
        ToSf(aSettings.attributeFlags),
        aSettings.sRgbCapable
    };
}

// Cursor

inline
sf::Cursor::Type ToSf(Cursor::Type aType) {
    switch (aType) {
    case Cursor::Type::Arrow:
        return sf::Cursor::Arrow;

    case Cursor::Type::ArrowWait:
        return sf::Cursor::ArrowWait;

    case Cursor::Type::Wait:
        return sf::Cursor::Wait;

    case Cursor::Type::Text:
        return sf::Cursor::Text;

    case Cursor::Type::Hand:
        return sf::Cursor::Hand;

    case Cursor::Type::SizeHorizontal:
        return sf::Cursor::SizeHorizontal;

    case Cursor::Type::SizeVertical:
        return sf::Cursor::SizeVertical;

    case Cursor::Type::SizeTopLeftBottomRight:
        return sf::Cursor::SizeTopLeftBottomRight;

    case Cursor::Type::SizeBottomLeftTopRight:
        return sf::Cursor::SizeBottomLeftTopRight;

    case Cursor::Type::SizeAll:
        return sf::Cursor::SizeAll;

    case Cursor::Type::Cross:
        return sf::Cursor::Cross;

    case Cursor::Type::Help:
        return sf::Cursor::Help;

    case Cursor::Type::NotAllowed:      
        return sf::Cursor::NotAllowed;

    default:
        HARD_ASSERT(false && "Invalid hg::gr::Cursor::Type value.");
   }
}

// VideoMode

inline
VideoMode ToHg(const sf::VideoMode& aVideoMode) {
    return {
        static_cast<PZInteger>(aVideoMode.width),
        static_cast<PZInteger>(aVideoMode.height),
        static_cast<PZInteger>(aVideoMode.bitsPerPixel)
    };
}

inline
sf::VideoMode ToSf(const VideoMode& aVideoMode) {
    return {
        static_cast<unsigned>(aVideoMode.width),
        static_cast<unsigned>(aVideoMode.height),
        static_cast<unsigned>(aVideoMode.bitsPerPixel)
    };
}

// WindowStyle

inline
int ToSf(WindowStyle aStyle) {
    if (aStyle == WindowStyle::None) {
        return sf::Style::None;
    }

    int result = 0;

    if ((aStyle & WindowStyle::Titlebar)   != WindowStyle::None) result |= sf::Style::Titlebar;
    if ((aStyle & WindowStyle::Resize)     != WindowStyle::None) result |= sf::Style::Resize;
    if ((aStyle & WindowStyle::Close)      != WindowStyle::None) result |= sf::Style::Close;
    if ((aStyle & WindowStyle::Fullscreen) != WindowStyle::None) result |= sf::Style::Fullscreen;
    if ((aStyle & WindowStyle::Default)    != WindowStyle::None) result |= sf::Style::Default;

    return result;
}

} // namespace win
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !HOBGOBLIN_GRAPHICS_SFML_CONVERSIONS_HPP
