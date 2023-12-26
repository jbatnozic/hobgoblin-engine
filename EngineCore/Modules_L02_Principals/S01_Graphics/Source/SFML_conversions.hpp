#ifndef HOBGOBLIN_GRAPHICS_SFML_CONVERSIONS_HPP
#define HOBGOBLIN_GRAPHICS_SFML_CONVERSIONS_HPP

// Hobgoblin

#include <Hobgoblin/HGExcept.hpp>

#include <Hobgoblin/Math/Rectangle.hpp>
#include <Hobgoblin/Math/Vector.hpp>
#include <Hobgoblin/Graphics/Blend_mode.hpp>
#include <Hobgoblin/Graphics/Color.hpp>
#include <Hobgoblin/Graphics/Primitive_type.hpp>
#include <Hobgoblin/Graphics/Render_states.hpp>
#include <Hobgoblin/Graphics/Render_target.hpp>
#include <Hobgoblin/Graphics/Shader.hpp>
#include <Hobgoblin/Graphics/Sprite.hpp>
#include <Hobgoblin/Graphics/Texture.hpp>
#include <Hobgoblin/Graphics/Texture_rect.hpp>
#include <Hobgoblin/Graphics/Transform.hpp>
#include <Hobgoblin/Graphics/Vertex_buffer.hpp>
#include <Hobgoblin/Graphics/View.hpp>
#include <Hobgoblin/Window/Context_settings.hpp>

// SFML

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/ContextSettings.hpp>

#include <filesystem>

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

template <class taArithmetic>
typename sf::Rect<taArithmetic> ToSf(const typename math::Rectangle<taArithmetic>& aRect) {
    return {aRect.getLeft(), aRect.getTop(), aRect.w, aRect.h};
}

template <class taArithmetic>
typename math::Rectangle<taArithmetic> ToHg(const typename sf::Rect<taArithmetic>& aRect) {
    return {aRect.left, aRect.top, aRect.width, aRect.height};
}

///////////////////////////////////////////////////////////////////////////
// FILSYSTEM                                                             //
///////////////////////////////////////////////////////////////////////////

inline
std::string FilesystemPathToSfPath(const std::filesystem::path& aPath) {
    return aPath.string();
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

using win::ContextSettings;

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

// TextureRect

sf::IntRect ConvertTextureRect(TextureRect aTextureRect);

// Transform

const sf::Transform& ToSf(const Transform& aTransform);

// VertexBuffer

VertexBuffer::Usage ToHg(sf::VertexBuffer::Usage aUsage);
sf::VertexBuffer::Usage ToSf(VertexBuffer::Usage aUsage);

// View

View ToHg(const sf::View& aView);
const sf::View& ToSf(const View& aView);

///////////////////////////////////////////////////////////////////////////
// INLINE DEFINITIONS                                                    //
///////////////////////////////////////////////////////////////////////////

// BlendMode

inline
BlendMode::Factor ToHg(sf::BlendMode::Factor aFactor) {
    switch (aFactor) {
    case sf::BlendMode::Zero: return BlendMode::Factor::Zero;
    case sf::BlendMode::One: return BlendMode::Factor::One;
    case sf::BlendMode::SrcColor: return BlendMode::Factor::SrcColor;
    case sf::BlendMode::OneMinusSrcColor: return BlendMode::Factor::OneMinusSrcColor;
    case sf::BlendMode::DstColor: return BlendMode::Factor::DstColor;
    case sf::BlendMode::OneMinusDstColor: return BlendMode::Factor::OneMinusDstColor;
    case sf::BlendMode::SrcAlpha: return BlendMode::Factor::SrcAlpha;
    case sf::BlendMode::OneMinusSrcAlpha: return BlendMode::Factor::OneMinusSrcAlpha;
    case sf::BlendMode::DstAlpha: return BlendMode::Factor::DstAlpha;
    case sf::BlendMode::OneMinusDstAlpha: return BlendMode::Factor::OneMinusDstAlpha;
    default:
        HG_UNREACHABLE("Invalid sf::BlendMode::Factor value ({}).", (int)aFactor);
    }
}

inline
BlendMode::Equation ToHg(sf::BlendMode::Equation aEquation) {
    switch (aEquation) {
    case sf::BlendMode::Add: return BlendMode::Equation::Add;
    case sf::BlendMode::Subtract: return BlendMode::Equation::Subtract;
    case sf::BlendMode::ReverseSubtract: return BlendMode::Equation::ReverseSubtract;
    default:
        HG_UNREACHABLE("Invalid sf::BlendMode::Equation value ({}).", (int)aEquation);
    }
}

inline
BlendMode ToHg(sf::BlendMode aBlendMode) {
    return {
        ToHg(aBlendMode.colorSrcFactor),
        ToHg(aBlendMode.colorDstFactor),
        ToHg(aBlendMode.colorEquation),
        ToHg(aBlendMode.alphaSrcFactor),
        ToHg(aBlendMode.alphaDstFactor),
        ToHg(aBlendMode.alphaEquation)
    };
}

inline
sf::BlendMode::Factor ToSf(BlendMode::Factor aFactor) {
    switch (aFactor) {
    case BlendMode::Factor::Zero: return sf::BlendMode::Zero;
    case BlendMode::Factor::One: return sf::BlendMode::One;
    case BlendMode::Factor::SrcColor: return sf::BlendMode::SrcColor;
    case BlendMode::Factor::OneMinusSrcColor: return sf::BlendMode::OneMinusSrcColor;
    case BlendMode::Factor::DstColor: return sf::BlendMode::DstColor;
    case BlendMode::Factor::OneMinusDstColor: return sf::BlendMode::OneMinusDstColor;
    case BlendMode::Factor::SrcAlpha: return sf::BlendMode::SrcAlpha;
    case BlendMode::Factor::OneMinusSrcAlpha: return sf::BlendMode::OneMinusSrcAlpha;
    case BlendMode::Factor::DstAlpha: return sf::BlendMode::DstAlpha;
    case BlendMode::Factor::OneMinusDstAlpha: return sf::BlendMode::OneMinusDstAlpha;
    default:
        HG_UNREACHABLE("Invalid hg::gr::BlendMode::Factor value ({}).", (int)aFactor);
    }
}

inline
sf::BlendMode::Equation ToSf(BlendMode::Equation aEquation) {
    switch (aEquation) {
    case BlendMode::Equation::Add: return sf::BlendMode::Add;
    case BlendMode::Equation::Subtract: return sf::BlendMode::Subtract;
    case BlendMode::Equation::ReverseSubtract: return sf::BlendMode::ReverseSubtract;
    default:
        HG_UNREACHABLE("Invalid hg::gr::BlendMode::Equation value ({}).", (int)aEquation);
    }
}

inline
sf::BlendMode ToSf(BlendMode aBlendMode) {
    return {
        ToSf(aBlendMode.colorSrcFactor),
        ToSf(aBlendMode.colorDstFactor),
        ToSf(aBlendMode.colorEquation),
        ToSf(aBlendMode.alphaSrcFactor),
        ToSf(aBlendMode.alphaDstFactor),
        ToSf(aBlendMode.alphaEquation)
    };
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
        HG_UNREACHABLE("Invalid sf::PrimitiveType value ({}).", (int)aType);
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
        HG_UNREACHABLE("Invalid hg::gr::PrimitiveType value ({}).", (int)aType);
    }
}

// RenderStates

inline
sf::RenderStates ToSf(const RenderStates& aRenderStates) {
    sf::RenderStates rs;

    rs.blendMode = ToSf(aRenderStates.blendMode);
    rs.transform = ToSf(aRenderStates.transform);
    rs.texture   = (aRenderStates.texture != nullptr) ? &detail::GraphicsImplAccessor::getImplOf<sf::Texture>(*aRenderStates.texture)
                                                      : nullptr;
    rs.shader    = (aRenderStates.shader != nullptr) ? &detail::GraphicsImplAccessor::getImplOf<sf::Shader>(*aRenderStates.shader)
                                                     : nullptr;
    
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
        HG_UNREACHABLE("Invalid hg::gr::Texture::CoordinateType value ({}).", (int)aCoordType);
    };
}

// TextureRect

inline
sf::IntRect ConvertTextureRect(TextureRect aTextureRect) {
    return {
        static_cast<int>(aTextureRect.getLeft()),
        static_cast<int>(aTextureRect.getTop()),
        static_cast<int>(aTextureRect.w),
        static_cast<int>(aTextureRect.h)
    };
}

// Transform

inline
Transform ToHg(const sf::Transform& aTransform) {
    // TODO(optimization: Improve efficiency of this function (private ctor??))
    Transform result;
    auto& sfTransform = detail::GraphicsImplAccessor::getImplOf<sf::Transform>(result);
    sfTransform = aTransform;
    return result;
}

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
        HG_UNREACHABLE("Invalid sf::VertexBuffer::Usage value ({}).", (int)aUsage);
    }
}

inline
sf::VertexBuffer::Usage ToSf(VertexBuffer::Usage aUsage) {
    switch (aUsage) {
    case VertexBuffer::Usage::Stream:  return sf::VertexBuffer::Stream;
    case VertexBuffer::Usage::Dynamic: return sf::VertexBuffer::Dynamic;
    case VertexBuffer::Usage::Static:  return sf::VertexBuffer::Static;
    default:
        HG_UNREACHABLE("Invalid hg::gr::VertexBuffer::Usage value ({}).", (int)aUsage);
    }
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

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !HOBGOBLIN_GRAPHICS_SFML_CONVERSIONS_HPP
