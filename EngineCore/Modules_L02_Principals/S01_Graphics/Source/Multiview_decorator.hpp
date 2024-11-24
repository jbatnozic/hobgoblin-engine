// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_GRAPHICS_MULTIVIEW_DECORATOR_HPP
#define UHOBGOBLIN_GRAPHICS_MULTIVIEW_DECORATOR_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Canvas.hpp>
#include <Hobgoblin/Graphics/View.hpp>

#include <cassert>
#include <utility>
#include <variant>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

template <class T>
class MultiViewDecorator final : public T {
    using Super = T;

public:
    template <class... taArgs>
    MultiViewDecorator(taArgs&&... aArgs)
        : Super{std::forward<taArgs>(aArgs)...}
        , _views{Super::getDefaultView()}
        , _viewCount{1} {}

    ///////////////////////////////////////////////////////////////////////////
    // CANVAS - BASIC                                                        //
    ///////////////////////////////////////////////////////////////////////////

    // All inherited

    ///////////////////////////////////////////////////////////////////////////
    // CANVAS - DRAWING                                                      //
    ///////////////////////////////////////////////////////////////////////////

    // void clear(const Color& aColor) override; // Inherit

    void draw(const Drawable& aDrawable, const RenderStates& aStates) override {
        for (PZInteger i = 0; i < _viewCount; i += 1) {
            const auto& view = getView(i);
            if (view.isEnabled()) {
                Super::setView(view);
                Super::draw(aDrawable, aStates);
            }
        }
    }

    void draw(const Vertex*       aVertices,
              PZInteger           aVertexCount,
              PrimitiveType       aPrimitiveType,
              const RenderStates& aStates) override {
        for (PZInteger i = 0; i < _viewCount; i += 1) {
            const auto& view = getView(i);
            if (view.isEnabled()) {
                Super::setView(view);
                Super::draw(aVertices, aVertexCount, aPrimitiveType, aStates);
            }
        }
    }

    void draw(const VertexBuffer& aVertexBuffer, const RenderStates& aStates) override {
        for (PZInteger i = 0; i < _viewCount; i += 1) {
            const auto& view = getView(i);
            if (view.isEnabled()) {
                Super::setView(view);
                Super::draw(aVertexBuffer, aStates);
            }
        }
    }

    void draw(const VertexBuffer& aVertexBuffer,
              PZInteger           aFirstVertex,
              PZInteger           aVertexCount,
              const RenderStates& aStates) override {
        for (PZInteger i = 0; i < _viewCount; i += 1) {
            const auto& view = getView(i);
            if (view.isEnabled()) {
                Super::setView(view);
                Super::draw(aVertexBuffer, aFirstVertex, aVertexCount, aStates);
            }
        }
    }

    // void flush() override; // Inherit

    ///////////////////////////////////////////////////////////////////////////
    // CANVAS - OPEN GL                                                      //
    ///////////////////////////////////////////////////////////////////////////

    // All inherited

    ///////////////////////////////////////////////////////////////////////////
    // VIEW CONTROLLER                                                       //
    ///////////////////////////////////////////////////////////////////////////

    void setViewCount(PZInteger aViewCount) override final {
        if (aViewCount < 1) {
            aViewCount = 1;
        }

        if (aViewCount > 1) {
            if (_viewCount > 1) {
                std::get<Views>(_views).resize(pztos(aViewCount));
            } else {
                View currentView = *_addressOfFirstView();
                _views           = Views{};
                std::get<Views>(_views).resize(pztos(aViewCount));
                std::get<Views>(_views)[0] = currentView;
            }
        } else {
            _views = *_addressOfFirstView();
        }

        _viewCount = aViewCount;
    }

    void setView(const View& aView) override final {
        getView(0) = aView;
    }

    void setView(PZInteger aViewIdx, const View& aView) override final {
        getView(aViewIdx) = aView;
    }

    PZInteger getViewCount() const override final {
        return _viewCount;
    }

    const View& getView(PZInteger aViewIdx = 0) const override final {
        assert(aViewIdx < _viewCount);
        return *(_addressOfFirstView() + aViewIdx);
    }

    View& getView(PZInteger aViewIdx = 0) override final {
        assert(aViewIdx < _viewCount);
        return *(_addressOfFirstView() + aViewIdx);
    }

    // View getDefaultView() const override final; // Inherit

    // math::Rectangle<PZInteger> getViewport(const View& aView) const override final; // Inherit
    using Super::getViewport;

    math::Rectangle<PZInteger> getViewport(PZInteger aViewIdx = 0) const override final {
        return Super::getViewport(getView(aViewIdx));
    }

    // math::Vector2f mapPixelToCoords(const math::Vector2i& aPoint,
    //                                 const View&            aView) const override final; // Inherit
    using Super::mapPixelToCoords;

    math::Vector2f mapPixelToCoords(const math::Vector2i& aPoint,
                                    PZInteger             aViewIdx = 0) const override final {
        return Super::mapPixelToCoords(aPoint, getView(aViewIdx));
    }

    // math::Vector2i mapCoordsToPixel(const math::Vector2f& aPoint,
    //                                 const View&           aView) const override final; // Inherit
    using Super::mapCoordsToPixel;

    math::Vector2i mapCoordsToPixel(const math::Vector2f& aPoint,
                                    PZInteger             aViewIdx = 0) const override final {
        return Super::mapCoordsToPixel(aPoint, getView(aViewIdx));
    }

private:
    using Views = std::vector<View>;

    std::variant<View, Views> _views;
    PZInteger                 _viewCount;

    View* _addressOfFirstView() {
        if (_viewCount > 1) {
            return std::get<Views>(_views).data();
        } else {
            return std::addressof(std::get<View>(_views));
        }
    }

    const View* _addressOfFirstView() const {
        if (_viewCount > 1) {
            return std::get<Views>(_views).data();
        } else {
            return std::addressof(std::get<View>(_views));
        }
    }
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_MULTIVIEW_DECORATOR_HPP
