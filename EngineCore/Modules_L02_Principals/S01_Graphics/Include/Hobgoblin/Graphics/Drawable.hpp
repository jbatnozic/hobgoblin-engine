// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_GR_DRAWABLE_HPP
#define UHOBGOBLIN_GR_DRAWABLE_HPP

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class Canvas;
class Drawable;
class RenderStates;

class Drawable {
public:
    enum class BatchingType {
        //! Object is a hg::gr::Vertex buffer.
        VertexBuffer,
        //! Object is a hg::gr::VertexArray.
        VertexArray,
        //! Object is a hg::gr::Sprite.
        Sprite,
        //! Object is a custom type whose _draw() method comes down to drawing
        //! one or more basic drawables (VertexBuffer, VertexArray or Sprite).
        Aggregate,
        //! Object is a custom type that doesn't implement its _draw() method
        //! through basic drawables (one possibility is that it calls OpenGL
        //! functions directly). Batching is not supported for these objects.
        Custom
    };

    virtual ~Drawable() = default;

    virtual BatchingType getBatchingType() const = 0;

private:
    virtual void _draw(Canvas& aCanvas, const RenderStates& aStates) const = 0;

    friend class DrawBatcher;
    friend class SfmlRenderTargetAdapter;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_DRAWABLE_HPP

// clang-format on
