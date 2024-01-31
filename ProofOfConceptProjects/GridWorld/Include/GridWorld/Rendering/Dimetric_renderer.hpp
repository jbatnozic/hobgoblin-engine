#pragma once

#include <Hobgoblin/Graphics/Sprite.hpp>
#include <Hobgoblin/Graphics/Sprite_loader.hpp>
#include <Hobgoblin/Graphics/View.hpp>

#include <GridWorld/Renderer.hpp>
#include <GridWorld/World/World.hpp>

#include <set>
#include <unordered_map>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

class DimetricRenderer /*: public Renderer*/ {
public:
    DimetricRenderer(const World& aWorld,
                     const hg::gr::SpriteLoader& aSpriteLoader)
        : _world{aWorld}
        , _spriteLoader{aSpriteLoader}
    {
    }

    void start(const hg::gr::View& aView, hg::math::Vector2f aPointOfView);

    void render(hg::gr::Canvas& aCanvas);

private:
    const World& _world;
    const hg::gr::SpriteLoader& _spriteLoader;

    mutable std::unordered_map<model::SpriteId, hg::gr::Sprite> _spriteCache;

    struct ViewData {
        hg::math::Vector2f center;
        hg::math::Vector2f size;

        hg::math::Vector2f isometricTopLeft;
        hg::math::Vector2f isometricBottomRight;
    };

    ViewData _viewData;

    struct RenderedObjectPtrLess {
        bool operator()(const RenderedObject* aLhs, const RenderedObject* aRhs) const;
    };

    std::set<const RenderedObject*, RenderedObjectPtrLess> _objects;

    hg::gr::Sprite& _getSprite(model::SpriteId aSpriteId) const;

    template <class taCallable>
    static void _diagonalTraverse(const World& aWorld, const ViewData& aViewData, taCallable&& aFunc);

    void _renderFloor(hg::gr::Canvas& aCanvas) const;
};

} // namespace gridworld
