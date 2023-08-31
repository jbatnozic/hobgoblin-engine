#ifndef HOBGOBLIN_GRAPHICS_SFML_CONVERSIONS_HPP
#define HOBGOBLIN_GRAPHICS_SFML_CONVERSIONS_HPP

// Hobgoblin

#include <Hobgoblin/Math/Vector.hpp>
#include <Hobgoblin/Graphics/Blend_mode.hpp>
#include <Hobgoblin/Graphics/Color.hpp>
#include <Hobgoblin/Graphics/Context_settings.hpp>
#include <Hobgoblin/Graphics/Primitive_type.hpp>
#include <Hobgoblin/Graphics/Render_states.hpp>
#include <Hobgoblin/Graphics/Render_target.hpp>
#include <Hobgoblin/Graphics/Sprite.hpp>
#include <Hobgoblin/Graphics/Texture.hpp>
#include <Hobgoblin/Graphics/Transform.hpp>
#include <Hobgoblin/Graphics/Vertex_buffer.hpp>
#include <Hobgoblin/Graphics/Video_mode.hpp>
#include <Hobgoblin/Graphics/View.hpp>
#include <Hobgoblin/Graphics/Window_style.hpp>

// SFML

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/ContextSettings.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/WindowStyle.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

namespace detail {
class GraphicsImplAccessor {
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

// BlendMode

BlendMode ToHg(sf::BlendMode aBlendMode);
sf::BlendMode ToSf(BlendMode aBlendMode);

// Color

Color ToHg(sf::Color aColor);
sf::Color ToSf(Color aColor);

// ContextSettings

ContextSettings::Attribute ToHg(sf::ContextSettings::Attribute aAttribute);
unsigned ToSf(ContextSettings::Attribute aAttribute);

ContextSettings ToHg(const sf::ContextSettings& aSettings);
sf::ContextSettings ToSf(const ContextSettings& aSettings);

// PrimitiveType

PrimitiveType ToHg(sf::PrimitiveType aType);
sf::PrimitiveType ToSf(PrimitiveType aType);

// RenderStates

sf::RenderStates ToSf(const RenderStates& aRenderStates);

// Sprite

const sf::Sprite& ToSf(const Sprite& aSprite);

// Texture

const sf::Texture& ToSf(const Texture& aTexture);

sf::Texture::CoordinateType ToSf(Texture::CoordinateType aCoordType);

// Transform

const sf::Transform& ToSf(const Transform& aTransform);

// VertexBuffer

VertexBuffer::Usage ToHg(sf::VertexBuffer::Usage aUsage);
sf::VertexBuffer::Usage ToSf(VertexBuffer::Usage aUsage);

// VideoMode

VideoMode ToHg(const sf::VideoMode& aVideoMode);
sf::VideoMode ToSf(const VideoMode& aVideoMode);

// View

View ToHg(const sf::View& aView);
const sf::View& ToSf(const View& aView);

// WindowStyle

int ToSf(WindowStyle aStyle);

///////////////////////////////////////////////////////////////////////////
// INLINE DEFINITIONS                                                    //
///////////////////////////////////////////////////////////////////////////

// BlendMode

inline
BlendMode ToHg(sf::BlendMode aBlendMode) {
    return {}; // TODO
}

inline
sf::BlendMode ToSf(BlendMode aBlendMode) {
    return {}; // TODO
}

// Color

inline
Color ToHg(sf::Color aColor) {
    return Color{aColor.toInteger()};
}

inline
sf::Color ToSf(Color aColor) {
    return sf::Color{aColor.toInt()};
}

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

// PrimitiveType

// NOTE: MSVC is too stupid to optimize these switch/case convertors (while clang and gcc can do it).

inline
PrimitiveType ToHg(sf::PrimitiveType aType) {
    switch (aType) {
    case sf::Points:        return PrimitiveType::Points;
    case sf::Lines:         return PrimitiveType::Lines;
    case sf::LineStrip:     return PrimitiveType::LineStrip;
    case sf::Triangles:     return PrimitiveType::Triangles;
    case sf::TriangleStrip: return PrimitiveType::TriangleStrip;
    case sf::TriangleFan:   return PrimitiveType::TriangleFan;
    default:
        HARD_ASSERT(false && "Invalid sf::PrimitiveType value.");
    }
}

inline
sf::PrimitiveType ToSf(PrimitiveType aType) {
    switch (aType) {
    case PrimitiveType::Points:        return sf::Points;
    case PrimitiveType::Lines:         return sf::Lines;
    case PrimitiveType::LineStrip:     return sf::LineStrip;
    case PrimitiveType::Triangles:     return sf::Triangles;
    case PrimitiveType::TriangleStrip: return sf::TriangleStrip;
    case PrimitiveType::TriangleFan:   return sf::TriangleFan;
    default:
        HARD_ASSERT(false && "Invalid hg::gr::PrimitiveType value.");
    }
}

// RenderStates

inline
sf::RenderStates ToSf(const RenderStates& aRenderStates) {
    sf::RenderStates rs;
    // TODO: other fields
    rs.transform = ToSf(aRenderStates.transform);
    return rs;
}

// Sprite

inline
const sf::Sprite& ToSf(const Sprite& aSprite) {
    const auto& sfSprite = detail::GraphicsImplAccessor::getImplOf<sf::Sprite>(aSprite);
    return sfSprite;
}

// Texture

inline
const sf::Texture& ToSf(const Texture& aTexture) {
    const auto& sfTexture = detail::GraphicsImplAccessor::getImplOf<sf::Texture>(aTexture);
    return sfTexture;
}

inline
sf::Texture::CoordinateType ToSf(Texture::CoordinateType aCoordType) {
    switch (aCoordType) {
    case Texture::CoordinateType::Normalized: return sf::Texture::Normalized;
    case Texture::CoordinateType::Pixels: return sf::Texture::Pixels;
    default:
        HARD_ASSERT(false && "Invalid hg::gr::Texture::CoordinateType value.");
    };
}

// Transform

inline
const sf::Transform& ToSf(const Transform& aTransform) {
    const auto& sfTransform = detail::GraphicsImplAccessor::getImplOf<sf::Transform>(aTransform);
    return sfTransform;
}

// VertexBuffer

inline
VertexBuffer::Usage ToHg(sf::VertexBuffer::Usage aUsage) {
    switch (aUsage) {
    case sf::VertexBuffer::Stream:  return VertexBuffer::Usage::Stream;
    case sf::VertexBuffer::Dynamic: return VertexBuffer::Usage::Dynamic;
    case sf::VertexBuffer::Static:  return VertexBuffer::Usage::Static;
    default:
        HARD_ASSERT(false && "Invalid sf::VertexBuffer::Usage value.");
    }
}

inline
sf::VertexBuffer::Usage ToSf(VertexBuffer::Usage aUsage) {
    switch (aUsage) {
    case VertexBuffer::Usage::Stream:  return sf::VertexBuffer::Stream;
    case VertexBuffer::Usage::Dynamic: return sf::VertexBuffer::Dynamic;
    case VertexBuffer::Usage::Static:  return sf::VertexBuffer::Static;
    default:
        HARD_ASSERT(false && "Invalid hg::gr::VertexBuffer::Usage value.");
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

// View

inline
View ToHg(const sf::View& aView) {
    View result;
    auto& sfView = detail::GraphicsImplAccessor::getImplOf<sf::View>(result);
    sfView = aView; // TODO: unnecessary copy when objects are bitwise compatible
    return result;
}

inline
const sf::View& ToSf(const View& aView) {
    const auto& sfView = detail::GraphicsImplAccessor::getImplOf<sf::View>(aView);
    return sfView;
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

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !HOBGOBLIN_GRAPHICS_SFML_CONVERSIONS_HPP
