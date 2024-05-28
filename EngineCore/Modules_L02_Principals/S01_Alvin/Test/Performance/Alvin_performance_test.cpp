// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Alvin.hpp>
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Math.hpp>

#include <chrono>
#include <iostream>
#include <list>

namespace alvin = hg::alvin;
namespace math  = hg::math;

using hg::NeverNull;

namespace {

constexpr auto LOG_ID = "Alvin.PerformanceTest";

constexpr float CELL_RESOLUTION = 32.f;

enum EntityIds {
    EID_BALL,
    EID_WALL,
    EID_SENSOR
};

enum EntityCategories {
    CAT_BALL   = 0x01,
    CAT_WALL   = 0x02,
    CAT_SENSOR = 0x04
};

class BallInterface : public alvin::EntityBase {
public:
    using EntitySuperclass = alvin::EntityBase;

    static constexpr alvin::EntityTypeId ENTITY_TYPE_ID = EID_BALL;

    static constexpr cpBitmask ENTITY_DEFAULT_CATEGORY = CAT_BALL;
    static constexpr cpBitmask ENTITY_DEFAULT_MASK     = CP_ALL_CATEGORIES;

    virtual void destroy() = 0;
};

class WallInterface : public alvin::EntityBase {
public:
    using EntitySuperclass = alvin::EntityBase;

    static constexpr alvin::EntityTypeId ENTITY_TYPE_ID = EID_WALL;

    static constexpr cpBitmask ENTITY_DEFAULT_CATEGORY = EID_WALL;
    static constexpr cpBitmask ENTITY_DEFAULT_MASK     = CAT_BALL;
};

class SensorInterface : public alvin::EntityBase {
public:
    using EntitySuperclass = alvin::EntityBase;

    static constexpr alvin::EntityTypeId ENTITY_TYPE_ID = EID_SENSOR;

    static constexpr cpBitmask ENTITY_DEFAULT_CATEGORY = CAT_SENSOR;
    static constexpr cpBitmask ENTITY_DEFAULT_MASK     = CP_ALL_CATEGORIES;
};

void Init(alvin::MainCollisionDispatcher& aDispatcher, NeverNull<cpSpace*> aSpace) {
    aDispatcher.registerEntityType<BallInterface>();
    aDispatcher.registerEntityType<WallInterface>();
    aDispatcher.configureSpace(aSpace);
}

///////////////////////////////////////////////////////////////////////////////

NeverNull<cpShape*> GetOtherShape(NeverNull<cpArbiter*> aArbiter, cpShape* aMyShape) {
    CP_ARBITER_GET_SHAPES(aArbiter, shape1, shape2);
    if (shape1 == aMyShape) {
        return shape2;
    }
    return shape1;
}

class Ball : public BallInterface {
public:
    Ball(NeverNull<cpSpace*> aSpace, math::Vector2d aPosition)
        : _colDelegate{_initColDelegate()}
        , _body{alvin::Body::createDynamic(1000.0, cpMomentForCircle(1000.0, 0.0, RADIUS, cpvzero))}
        , _shape{alvin::Shape::createCircle(_body, RADIUS, cpvzero)} {
        _colDelegate.bind(*this, _shape);
        cpSpaceAddBody(aSpace, _body);
        setPosition(aPosition);
        cpSpaceAddShape(aSpace, _shape);
    }

    Ball(Ball&&) = delete;

    void addListInfo(std::list<Ball>& aList, std::list<Ball>::iterator aIter) {
        _ballList     = &aList;
        _ballListIter = aIter;
    }

    void setPosition(math::Vector2d aPosition) {
        cpBodySetPosition(_body, cpv(aPosition.x, aPosition.y));
    }

    NeverNull<const cpShape*> getShape() const {
        return _shape;
    }

    void draw(hg::gr::Canvas& aCanvas) const {
        hg::gr::CircleShape circle{RADIUS};
        circle.setFillColor(hg::gr::COLOR_WHITE);
        circle.setOutlineColor(hg::gr::COLOR_RED);
        circle.setOutlineThickness(1.f);

        const auto pos = cpBodyGetPosition(_body);
        circle.setOrigin({RADIUS, RADIUS});
        circle.setPosition({(float)pos.x, (float)pos.y});

        aCanvas.draw(circle);
    }

    void destroy() override {
        _ballList->erase(_ballListIter);
    }

private:
    static constexpr float RADIUS = CELL_RESOLUTION / 2.f;

    alvin::CollisionDelegate _colDelegate;
    alvin::Body              _body;
    alvin::Shape             _shape;

    std::list<Ball>*          _ballList     = nullptr;
    std::list<Ball>::iterator _ballListIter = {};

    alvin::CollisionDelegate _initColDelegate() {
        auto builder = alvin::CollisionDelegateBuilder{};
        builder.addInteraction<WallInterface>(alvin::COLLISION_CONTACT,
                                              [](WallInterface&        aOther,
                                                 NeverNull<cpArbiter*> aArbiter,
                                                 NeverNull<cpSpace*>   aSpace,
                                                 hg::PZInteger         aOrder) -> alvin::Decision {
                                                  return alvin::Decision::ACCEPT_COLLISION;
                                              });
        builder.addInteraction<WallInterface>(alvin::COLLISION_PRE_SOLVE,
                                              [](WallInterface&        aOther,
                                                 NeverNull<cpArbiter*> aArbiter,
                                                 NeverNull<cpSpace*>   aSpace,
                                                 hg::PZInteger         aOrder) -> alvin::Decision {
                                                  return alvin::Decision::ACCEPT_COLLISION;
                                              });
        builder.addInteraction<WallInterface>(alvin::COLLISION_POST_SOLVE,
                                              [](WallInterface&        aOther,
                                                 NeverNull<cpArbiter*> aArbiter,
                                                 NeverNull<cpSpace*>   aSpace,
                                                 hg::PZInteger         aOrder) {

                                              });
        builder.addInteraction<WallInterface>(alvin::COLLISION_SEPARATION,
                                              [](WallInterface&        aOther,
                                                 NeverNull<cpArbiter*> aArbiter,
                                                 NeverNull<cpSpace*>   aSpace,
                                                 hg::PZInteger         aOrder) {});
        return builder.finalize();
    }
};

class Wall : public WallInterface {
public:
    Wall(NeverNull<cpSpace*> aSpace, math::Vector2d aPosition)
        : _colDelegate{_initColDelegate()}
        , _body{alvin::Body::createStatic()}
        , _shape{alvin::Shape::createBox(_body, CELL_RESOLUTION, CELL_RESOLUTION, 0.0)} {
        _colDelegate.bind(*this, _shape);
        cpSpaceAddBody(aSpace, _body);
        setPosition(aPosition);
        cpSpaceAddShape(aSpace, _shape);
    }

    Wall(Wall&&) = delete;

    void setPosition(math::Vector2d aPosition) {
        cpBodySetPosition(_body, cpv(aPosition.x, aPosition.y));
    }

    NeverNull<const cpShape*> getShape() const {
        return _shape;
    }

    void draw(hg::gr::Canvas& aCanvas) const {
        hg::gr::RectangleShape rect{
            {CELL_RESOLUTION, CELL_RESOLUTION}
        };
        rect.setFillColor(hg::gr::COLOR_BLACK);

        const auto pos = cpBodyGetPosition(_body);
        rect.setOrigin({CELL_RESOLUTION / 2.f, CELL_RESOLUTION / 2.f});
        rect.setPosition({(float)pos.x, (float)pos.y});

        aCanvas.draw(rect);
    }

private:
    alvin::CollisionDelegate _colDelegate;
    alvin::Body              _body;
    alvin::Shape             _shape;

    alvin::CollisionDelegate _initColDelegate() {
        auto builder = alvin::CollisionDelegateBuilder{};
        return builder.finalize();
    }
};

class Sensor : public SensorInterface {
public:
    Sensor(NeverNull<cpSpace*> aSpace, math::Vector2d aPosition)
        : _colDelegate{_initColDelegate()}
        , _body{alvin::Body::createKinematic()}
        , _shape{alvin::Shape::createCircle(_body, RADIUS, cpvzero)} {
        cpShapeSetSensor(_shape, true);
        _colDelegate.bind(*this, _shape);
        cpSpaceAddBody(aSpace, _body);
        setPosition(aPosition);
        cpSpaceAddShape(aSpace, _shape);
    }

    Sensor(Sensor&&) = delete;

    void setPosition(math::Vector2d aPosition) {
        cpBodySetPosition(_body, cpv(aPosition.x, aPosition.y));
    }

    void draw(hg::gr::Canvas& aCanvas) const {
        hg::gr::CircleShape circle{RADIUS};
        circle.setFillColor(hg::gr::COLOR_TRANSPARENT);
        circle.setOutlineColor(hg::gr::COLOR_BLUE);
        circle.setOutlineThickness(2.f);

        const auto pos = cpBodyGetPosition(_body);
        circle.setOrigin({RADIUS, RADIUS});
        circle.setPosition({(float)pos.x, (float)pos.y});

        aCanvas.draw(circle);
    }

private:
    static constexpr float RADIUS = 64.0;

    alvin::CollisionDelegate _colDelegate;
    alvin::Body              _body;
    alvin::Shape             _shape;

    alvin::CollisionDelegate _initColDelegate() {
        auto builder = alvin::CollisionDelegateBuilder{};
        builder.addInteraction<BallInterface>(alvin::COLLISION_PRE_SOLVE,
                                              [](BallInterface&        aOther,
                                                 NeverNull<cpArbiter*> aArbiter,
                                                 NeverNull<cpSpace*>   aSpace,
                                                 hg::PZInteger         aOrder) -> alvin::Decision {
                                                  if (hg::in::CheckPressedPK(hg::in::PK_D)) {
                                                    cpSpaceAddPostStepCallback(aSpace, &_ps, &aOther, nullptr);
                                                  }
                                                  return alvin::Decision::ACCEPT_COLLISION;
                                              });
        return builder.finalize();
    }

    static void _ps(cpSpace* aSpace, void* aKey, void* aData) {
        auto* ball = static_cast<BallInterface*>(aKey);
        ball->destroy();
    }
};

#define CONTACT    (0x01 << alvin::detail::USAGE_COL_BEGIN)
#define PRE_SOLVE  (0x01 << alvin::detail::USAGE_COL_PRESOLVE)
#define POST_SOLVE (0x01 << alvin::detail::USAGE_COL_POSTSOLVE)
#define SEPARATE   (0x01 << alvin::detail::USAGE_COL_SEPARATE)

std::list<Wall> SetUpArenaEdges(hg::NeverNull<cpSpace*> aSpace,
                                hg::PZInteger           aWidth,
                                hg::PZInteger           aHeight) {
    std::list<Wall> walls;
    for (hg::PZInteger y = 0; y < aHeight; y += 1) {
        for (hg::PZInteger x = 0; x < aWidth; x += 1) {
            if (!(x == 0 || y == 0 || x == aWidth - 1 || y == aHeight - 1)) {
                continue;
            }
            walls.emplace_back(aSpace,
                               math::Vector2d{(x + 0.5) * CELL_RESOLUTION, (y + 0.5) * CELL_RESOLUTION});
        }
    }
    return walls;
}

} // namespace

int main(int argc, char* argv[]) {
    hg::log::SetMinimalLogSeverity(hg::log::Severity::Info);

    hg::gr::RenderWindow window{hg::win::VideoMode::getDesktopMode(), "Wallz & Ballz"};
    window.setFramerateLimit(60);

    alvin::Space space;
    cpSpaceSetGravity(space, cpv(0.0, 500.0));

    alvin::MainCollisionDispatcher collisionDispatcher;
    Init(collisionDispatcher, space);

    std::list<Wall> walls = SetUpArenaEdges(space, 100, 50);
    std::list<Ball> balls = {};
    Sensor          sensor{space, {0.0, 0.0}};

    math::Vector2i mousePos   = {0, 0};
    bool           lmbPressed = false;
    bool           rmbPressed = false;

    while (window.isOpen()) {
        hg::win::Event ev;
        while (window.pollEvent(ev)) {
            ev.visit(
                [&window](const hg::win::Event::Closed&) {
                    window.close();
                },
                [&](const hg::win::Event::MouseButtonPressed& aData) {
                    mousePos = {aData.x, aData.y};
                    switch (aData.button) {
                    case hg::in::MB_LEFT: lmbPressed = true; break;
                    case hg::in::MB_RIGHT: rmbPressed = true; break;
                    default: break;
                    }
                },
                [&](const hg::win::Event::MouseButtonReleased& aData) {
                    mousePos = {aData.x, aData.y};
                    switch (aData.button) {
                    case hg::in::MB_LEFT: lmbPressed = false; break;
                    case hg::in::MB_RIGHT: rmbPressed = false; break;
                    default: break;
                    }
                },
                [&](const hg::win::Event::MouseMoved& aData) {
                    mousePos = {aData.x, aData.y};
                });
        }

        const auto worldPos = window.mapPixelToCoords(mousePos);
        if (lmbPressed) {
            balls.emplace_back(space, math::Vector2d{worldPos.x, worldPos.y});
            balls.back().addListInfo(balls, std::prev(balls.end()));
            HG_LOG_INFO(LOG_ID, "Ball count: {}", balls.size());
        }

        sensor.setPosition({worldPos.x, worldPos.y});

        const auto start = std::chrono::steady_clock::now();
        space.step(1.0 / 60.0);
        const auto end = std::chrono::steady_clock::now();
        if (hg::in::CheckPressedPK(hg::in::PK_SPACE)) {
            const auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0;
            HG_LOG_INFO(LOG_ID, "Step took {}ms.", ms);
        }

        window.clear(hg::gr::COLOR_GREY);

        for (const auto& ball : balls) {
            ball.draw(window);
        }
        for (const auto& wall : walls) {
            wall.draw(window);
        }
        sensor.draw(window);

        window.display();
    }

    return EXIT_SUCCESS;
}
