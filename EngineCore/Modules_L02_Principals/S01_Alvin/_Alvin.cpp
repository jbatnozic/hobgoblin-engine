#if 0
#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Window.hpp>

#include <Hobgoblin/Alvin/Body.hpp>
#include <Hobgoblin/Alvin/Collision_delegate.hpp>
#include <Hobgoblin/Alvin/Collision_delegate_builder.hpp>
#include <Hobgoblin/Alvin/Constraint.hpp>
#include <Hobgoblin/Alvin/Decision.hpp>
#include <Hobgoblin/Alvin/Entity_base.hpp>
#include <Hobgoblin/Alvin/Main_collision_dispatcher.hpp>
#include <Hobgoblin/Alvin/Shape.hpp>
#include <Hobgoblin/Alvin/Space.hpp>

#include <cstdint>
#include <iostream>
#include <stdexcept>

namespace hg    = jbatnozic::hobgoblin;
namespace alvin = hg::alvin;

enum Tags {
    TAG_CHARACTER,
    TAG_PLAYER,
    TAG_TERRAIN
};

class Character : public alvin::EntityBase {
public:
    using EntitySuperclass = alvin::EntityBase;

    static constexpr alvin::EntityTypeId ENTITY_TYPE_ID = TAG_CHARACTER;
};

class Player : public Character {
public:
    using EntitySuperclass = Character;

    static constexpr alvin::EntityTypeId ENTITY_TYPE_ID = TAG_PLAYER;
};

class Terrain : public alvin::EntityBase {
public:
    using EntitySuperclass = alvin::EntityBase;

    static constexpr alvin::EntityTypeId ENTITY_TYPE_ID = TAG_TERRAIN;
};

static_assert(Character::ENTITY_TYPE_ID == TAG_CHARACTER);
static_assert(Player::ENTITY_TYPE_ID == TAG_PLAYER);
static_assert(Terrain::ENTITY_TYPE_ID == TAG_TERRAIN);

///////////////////////////////////////////////////////////////////////////////

class PlayerDude : public Player {
public:
    PlayerDude(cpSpace* aSpace)
        : _body{cpBodyNew(100.f, cpMomentForCircle(100.f, 0.f, 5.f, cpv(0.f, 0.f)))}
        , _shape{cpCircleShapeNew(_body, 100.f, cpv(0.f, 0.f))}
        , _collisionDelegate{_createCollisionDelegate()} {
        _collisionDelegate.bind<Player>(*this, _shape);

        cpSpaceAddBody(aSpace, _body);
        cpSpaceAddShape(aSpace, _shape);
        cpBodySetPosition(_body, cpv(64.0, 64.0));
    }

    void drawOnto(hg::gr::Canvas& aCanvas) {
        hg::gr::CircleShape circle{100.f};
        circle.setOrigin({100.f, 100.f});
        circle.setFillColor(hg::gr::COLOR_TRANSPARENT);
        circle.setOutlineColor(hg::gr::COLOR_GREEN);
        circle.setOutlineThickness(1.f);

        const auto pos = cpBodyGetPosition(_body);
        circle.setPosition({(float)pos.x, (float)pos.y});
        aCanvas.draw(circle);

        hg::gr::RectangleShape rect;
        rect.setFillColor(hg::gr::COLOR_TRANSPARENT);
        rect.setOutlineColor(hg::gr::COLOR_RED);
        rect.setOutlineThickness(1.f);
        cpShapeCacheBB(_shape);
        const auto bbox = cpShapeGetBB(_shape);

        rect.setPosition({(float)bbox.l, (float)bbox.t});
        rect.setSize({(float)(bbox.r - bbox.l), (float)(bbox.b - bbox.t)});
        aCanvas.draw(rect);
    }

private:
    alvin::Body              _body;
    alvin::Shape             _shape;
    alvin::CollisionDelegate _collisionDelegate;

    alvin::CollisionDelegate _createCollisionDelegate() {
        return alvin::CollisionDelegateBuilder{}
            .addInteraction<Terrain>(alvin::COLLISION_CONTACT,
                                     [](Terrain&,
                                        HG_NEVER_NULL(cpArbiter*),
                                        HG_NEVER_NULL(cpSpace*),
                                        hg::PZInteger aOrder) -> alvin::Decision {
                                         return alvin::Decision::ACCEPT_COLLISION;
                                     })
            .finalize();
    }
};

class TerrainController : public Terrain {
public:
    TerrainController(alvin::Space& aSpace)
        : _space{aSpace}
        , _collisionDelegate{_createCollisionDelegate()} {
        auto* body = cpSpaceGetStaticBody(_space);

        // clang-format off
        cpVect vertices[] = {
            cpv(0.0, 0.0),
            cpv(0.0, 1.0),
            cpv(1.0, 1.0),
            cpv(1.0, 0.0),
            cpv(0.0, 0.0)
        };
        // clang-format on
        for (auto& vertex : vertices) {
            vertex.x *= 32.0;
            vertex.y *= 32.0;
        }

        for (int i = 0; i < 20; i += 1) {
            alvin::Shape shape = {
                cpPolyShapeNew(body, 5, vertices, cpTransformTranslate(cpv(i * 32.0, 640.0)), 0.0)};
            _space.add(shape);
            _shapes.push_back(std::move(shape));
        }

        _collisionDelegate.bind<Terrain>(*this, _shapes.begin(), _shapes.end());
    }

    void drawOnto(hg::gr::Canvas& aCanvas) {
        hg::gr::RectangleShape rect;
        rect.setFillColor(hg::gr::COLOR_TRANSPARENT);
        rect.setOutlineColor(hg::gr::COLOR_RED);
        rect.setOutlineThickness(1.f);

        for (auto& shape : _shapes) {
            cpShapeCacheBB(shape);
            const auto bbox = cpShapeGetBB(shape);

            rect.setPosition({(float)bbox.l, (float)bbox.t});
            rect.setSize({(float)(bbox.r - bbox.l), (float)(bbox.b - bbox.t)});
            aCanvas.draw(rect);
        }
    }

private:
    alvin::Space&             _space;
    alvin::CollisionDelegate  _collisionDelegate;
    std::vector<alvin::Shape> _shapes;

    alvin::CollisionDelegate _createCollisionDelegate() {
        return alvin::CollisionDelegateBuilder{}.finalize();
    }
};

int main(int argc, char* argv[]) try {
    alvin::Space space{};
    cpSpaceSetIterations(space, 30);
    cpSpaceSetGravity(space, cpv(0.0, 1000.0));

    alvin::MainCollisionDispatcher dispatcher;
    dispatcher.registerEntityType<Character>()
        .registerEntityType<Player>()
        .registerEntityType<Terrain>()
        .configureSpace(space);

    PlayerDude        dude{space};
    TerrainController terrain{space};

    ///////////////////////////////////////

    hg::gr::RenderWindow window{
        hg::win::VideoMode{800, 800},
        "Alvin & The Chipmunk",
        hg::win::WindowStyle::Default
    };
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        hg::win::Event ev;
        while (window.pollEvent(ev)) {
            ev.visit([&](const hg::win::Event::Closed& aData) {
                window.close();
            });
        }

        space.step(1.0 / 60.0);

        window.clear(hg::gr::COLOR_DARK_GREY);
        dude.drawOnto(window);
        terrain.drawOnto(window);
        window.display();
    }
} catch (const std::exception& ex) {
    std::cerr << "Exception caught: " << ex.what() << std::endl;
}
#endif
