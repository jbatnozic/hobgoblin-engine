
#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Window.hpp>

#include <Hobgoblin/Alvin/Collision_delegate.hpp>
#include <Hobgoblin/Alvin/Collision_delegate_builder.hpp>
#include <Hobgoblin/Alvin/Entity_base.hpp>
#include <Hobgoblin/Alvin/Main_collision_dispatcher.hpp>

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace hg = jbatnozic::hobgoblin;
namespace alvin = hg::alvin;

enum Tags {
    TAG_CHARACTER,
    TAG_PLAYER,
    TAG_TERRAIN
};

class Character : public alvin::EntityBase {
public:
    static constexpr std::int32_t ENTITY_TYPE_ID = TAG_CHARACTER;
    using EntitySuperclass = alvin::EntityBase;
};

class Player : public Character {
public:
    static constexpr std::int32_t ENTITY_TYPE_ID = TAG_PLAYER;
    using EntitySuperclass = Character;
};

class Terrain : public alvin::EntityBase {
public:
    static constexpr std::int32_t ENTITY_TYPE_ID = TAG_TERRAIN;
    using EntitySuperclass = alvin::EntityBase;
};

static_assert(Character::ENTITY_TYPE_ID == TAG_CHARACTER);
static_assert(Player::ENTITY_TYPE_ID    == TAG_PLAYER);
static_assert(Terrain::ENTITY_TYPE_ID   == TAG_TERRAIN);

///////////////////////////////////////////////////////////////////////////////

class PlayerDude : public Player {
public:
    PlayerDude(cpSpace* aSpace)
        : _collisionDelegate{_createCollisionDelegate()}
    {
        _body = cpBodyNew(100.f, cpMomentForCircle(100.f, 0.f, 5.f, cpv(0.f, 0.f)));
        _shape = cpCircleShapeNew(_body, 100.f, cpv(0.f, 0.f));

        //cpShapeSetCollisionType(_shape, 0x01);
        //cpShapeSetFilter(_shape, cpShapeFilterNew(CP_NO_GROUP, 0x01, CP_ALL_CATEGORIES));

        _collisionDelegate.bind<Player>(*this, _body);

        cpSpaceAddBody(aSpace, _body);
        cpSpaceAddShape(aSpace, _shape);
        cpBodySetPosition(_body, cpv(64.0, 64.0));
    }

    void drawOnto(hg::gr::Canvas& aCanvas) {
        hg::gr::CircleShape circle{100.f};
        circle.setOrigin({50.f, 50.f});
        circle.setFillColor(hg::gr::COLOR_TRANSPARENT);
        circle.setOutlineColor(hg::gr::COLOR_GREEN);
        circle.setOutlineThickness(1.f);
        circle.setOrigin({50.f, 50.f});

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
    alvin::CollisionDelegate _collisionDelegate;

    cpBody* _body;
    cpShape* _shape;

    alvin::CollisionDelegate _createCollisionDelegate() {
        return alvin::CollisionDelegateBuilder{}
            .addInteraction<Terrain>(alvin::COLLISION_CONTACT,
                                     [](Terrain&,
                                        HG_NEVER_NULL(cpArbiter*),
                                        HG_NEVER_NULL(cpSpace*),
                                        hg::PZInteger aOrder) -> bool {
                                         return true;
                                     })
            .finalize();
    }
};

class TerrainController : public Terrain {
public:
    TerrainController(cpSpace* aSpace)
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
            auto* shape =
                cpPolyShapeNew(body, 5, vertices, cpTransformTranslate(cpv(i * 32.0, 640.0)), 0.0);
            //cpShapeSetCollisionType(shape, 0x02);
            //cpShapeSetFilter(shape, cpShapeFilterNew(CP_NO_GROUP, 0x01, CP_ALL_CATEGORIES));
            cpSpaceAddShape(_space, shape);
            _shapes.push_back(shape);
        }

        _collisionDelegate.bind<Terrain>(*this, body);
    }

    void drawOnto(hg::gr::Canvas& aCanvas) {
        hg::gr::RectangleShape rect;
        rect.setFillColor(hg::gr::COLOR_TRANSPARENT);
        rect.setOutlineColor(hg::gr::COLOR_RED);
        rect.setOutlineThickness(1.f);

        for (const auto& shape : _shapes) {
            cpShapeCacheBB(shape);
            const auto bbox = cpShapeGetBB(shape);
            
            rect.setPosition({(float)bbox.l, (float)bbox.t});
            rect.setSize({(float)(bbox.r - bbox.l), (float)(bbox.b - bbox.t)});
            aCanvas.draw(rect);
        }
    }

private:
    cpSpace* _space;
    alvin::CollisionDelegate _collisionDelegate;
    std::vector<cpShape*> _shapes;

    alvin::CollisionDelegate _createCollisionDelegate() {
        return alvin::CollisionDelegateBuilder{}
            .finalize();
    }
};

int main(int argc, char* argv[]) try {
    cpSpace* space = cpSpaceNew();
    cpSpaceSetIterations(space, 30);
    cpSpaceSetGravity(space, cpv(0.0, 1000.0));

    alvin::MainCollisionDispatcher dispatcher;
    dispatcher
        .registerEntityType<Character>()
        .registerEntityType<Player>()
        .registerEntityType<Terrain>()
        .configureSpace(space);

    PlayerDude dude{space};
    TerrainController terrain{space};

    ///////////////////////////////////////   

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

        cpSpaceStep(space, 1.0 / 60.0);

        window.clear(hg::gr::COLOR_DARK_GREY);
        dude.drawOnto(window);
        terrain.drawOnto(window);
        window.display();
    }
}
catch (const std::exception& ex) {
    std::cerr << "Exception caught: " << ex.what() << std::endl;
}
