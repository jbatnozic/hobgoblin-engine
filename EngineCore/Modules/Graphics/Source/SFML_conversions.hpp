#ifndef HOBGOBLIN_GRAPHICS_SFML_CONVERSIONS_HPP
#define HOBGOBLIN_GRAPHICS_SFML_CONVERSIONS_HPP

// Hobgoblin

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
// DECLARATIONS                                                          //
///////////////////////////////////////////////////////////////////////////

// BlendMode

BlendMode ToHg(sf::BlendMode aBlendMode);
sf::BlendMode ToSf(BlendMode aBlendMode);

// Color

Color ToHg(sf::Color aColor);
sf::Color ToSf(Color aColor);

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
        HARD_ASSERT(false && "Invalid hg::gr::Texture::CoordinateType value.");
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
    // TODO: Improve efficiency of this function (private ctor??)
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
