
#include <cstdio>
#include <vector>

#include <chipmunk\chipmunk.h>
#include <chipmunk\chipmunk_structs.h>
#include <SFML\Graphics.hpp>

#include "RowMajorMatrix.hpp"

#define SHIP1_WIDTH  25
#define SHIP1_HEIGHT 50

#define SHIP2_WIDTH  30
#define SHIP2_HEIGHT 5

#define CP_ZERO_VECTOR cpv(0.0, 0.0)

constexpr cpFloat TILE_DIAMETER = 1.0;
constexpr cpFloat MAX_JOINT_FORCE = 850.0;

//constexpr int SIZE_TEST = sizeof(cpCircleShape);

namespace {
    cpSpace* space;
}

class ShipTile {
public:
    
    ShipTile(cpBody* body_ = nullptr, cpShape* shape_ = nullptr)
        : body{body_}
        , shape{shape_}
        , above_constraint{nullptr}
        , left_constraint{nullptr}
    {
    }

    ShipTile(const ShipTile& rhs) = delete;
    ShipTile& operator=(const ShipTile& rhs) = delete;

    ShipTile(ShipTile&& rhs) {
        body = rhs.body;
        shape = rhs.shape;
        rhs.body = nullptr;
        rhs.shape = nullptr;
    }

    ShipTile& operator=(ShipTile&& rhs) {
        if (this != &rhs) {
            body = rhs.body;
            shape = rhs.shape;
            rhs.body = nullptr;
            rhs.shape = nullptr;
        }
        return *this;
    }

    ~ShipTile() {
        destroy();
    }

    void reset(cpBody* body_, cpShape* shape_) {
        destroy();
        body = body_;
        shape = shape_;
    }

    void destroy() {
        setAboveConstraint(nullptr);
        setLeftConstraint(nullptr);
        if (body != nullptr && shape != nullptr) {
            cpSpace* space = cpBodyGetSpace(body);
            cpSpaceRemoveShape(space, shape);
            cpSpaceRemoveBody(space, body);
            body = nullptr;
            shape = nullptr;
        }
    }

    void setAboveConstraint(cpConstraint* constraint) {
        if (above_constraint != nullptr) {
            cpSpaceRemoveConstraint(space, above_constraint);
            cpConstraintFree(above_constraint);
        }
        above_constraint = constraint;
    }

    void setLeftConstraint(cpConstraint* constraint) {
        if (left_constraint != nullptr) {
            cpSpaceRemoveConstraint(space, left_constraint);
            cpConstraintFree(left_constraint);
        }
        left_constraint = constraint;
    }

    cpBody* getBody() const {
        return body;
    }

    cpShape* getShape() const {
        return shape;
    }

    void update() {
        cpFloat force;
        cpFloat maxForce;

        // Left:
        if (left_constraint) {
            force = cpConstraintGetImpulse(left_constraint) * 30.0;
            maxForce = cpConstraintGetMaxForce(left_constraint);
            if (force > 0.9 * maxForce) {
                setLeftConstraint(nullptr);
            }
        }

        // Above:
        if (above_constraint) {
            force = cpConstraintGetImpulse(above_constraint) * 30.0;
            maxForce = cpConstraintGetMaxForce(above_constraint);
            if (force > 0.9 * maxForce) {
                setAboveConstraint(nullptr);
            }
        }
    }

private:
    cpBody* body;
    cpShape* shape;
    cpConstraint* above_constraint;
    cpConstraint* left_constraint;
};

namespace {
    RowMajorMatrix<ShipTile> ship1_tiles{SHIP1_WIDTH, SHIP1_HEIGHT};
    RowMajorMatrix<ShipTile> ship2_tiles{SHIP2_WIDTH, SHIP2_HEIGHT};
}

void Initialize() {
    // Create space
    space = cpSpaceNew();
    //cpSpaceUseSpatialHash(space, TILE_DIAMETER * 2.0, SHIP_WIDTH * SHIP_HEIGHT * 10); // Experimental

    // Create tiles
    const cpFloat tile_mass = 1.0;
    const cpFloat tile_moment = cpMomentForCircle(tile_mass, 0.0, TILE_DIAMETER, cpv(0.0, 0.0));
    const cpFloat error_bias = 0.1;//cpfpow(0.9, 30.0);

    cpFloat xoff = 25.0;
    cpFloat yoff = 10.0;

    // Ship 1:
    for (int y = 0; y < SHIP1_HEIGHT; y += 1) {
        for (int x = 0; x < SHIP1_WIDTH; x += 1) {
            cpBody* body = cpSpaceAddBody(space, cpBodyNew(tile_mass, tile_moment));
            cpShape* shape = cpSpaceAddShape(space, cpCircleShapeNew(body, TILE_DIAMETER / 2.0, cpv(0.0, 0.0))); 
            
            cpBodySetPosition(body, cpv(xoff + x * TILE_DIAMETER, yoff + y * TILE_DIAMETER));
            ship1_tiles.at(x, y).reset(body, shape);

            if (x != 0) {
                cpConstraint* con = cpPinJointNew(body,
                                                  ship1_tiles.at(x - 1, y).getBody(),
                                                  cpv(-TILE_DIAMETER / 2.0, 0.0),
                                                  cpv(+TILE_DIAMETER / 2.0, 0.0));
                cpConstraintSetErrorBias(con, error_bias);
                cpConstraintSetMaxForce(con, MAX_JOINT_FORCE);
                cpConstraintSetCollideBodies(con, cpTrue);
                cpSpaceAddConstraint(space, con);
                ship1_tiles.at(x, y).setLeftConstraint(con);
            }

            if (y != 0) {
                cpConstraint* con = cpPinJointNew(body,
                                                  ship1_tiles.at(x, y - 1).getBody(),
                                                  cpv(0.0, -TILE_DIAMETER / 2.0),
                                                  cpv(0.0, +TILE_DIAMETER / 2.0));
                cpConstraintSetErrorBias(con, error_bias);
                cpConstraintSetMaxForce(con, MAX_JOINT_FORCE);
                cpConstraintSetCollideBodies(con, cpTrue);
                cpSpaceAddConstraint(space, con);
                ship1_tiles.at(x, y).setAboveConstraint(con);
            }
        }
    }

    // Ship 2:
    xoff = 100.0;
    yoff = 25.0;

    for (int y = 0; y < SHIP2_HEIGHT; y += 1) {
        for (int x = 0; x < SHIP2_WIDTH; x += 1) {
            cpBody* body = cpSpaceAddBody(space, cpBodyNew(tile_mass, tile_moment));
            cpShape* shape = cpSpaceAddShape(space, cpCircleShapeNew(body, TILE_DIAMETER / 2.0, cpv(0.0, 0.0))); 
            
            cpBodySetPosition(body, cpv(xoff + x * TILE_DIAMETER, yoff + y * TILE_DIAMETER));
            ship2_tiles.at(x, y).reset(body, shape);

            if (x != 0) {
                cpConstraint* con = cpPinJointNew(body,
                                                  ship2_tiles.at(x - 1, y).getBody(),
                                                  cpv(-TILE_DIAMETER / 2.0, 0.0),
                                                  cpv(+TILE_DIAMETER / 2.0, 0.0));
                cpConstraintSetErrorBias(con, error_bias);
                cpConstraintSetMaxForce(con, MAX_JOINT_FORCE);
                cpConstraintSetCollideBodies(con, cpTrue);
                cpSpaceAddConstraint(space, con);
                ship2_tiles.at(x, y).setLeftConstraint(con);
            }

            if (y != 0) {
                cpConstraint* con = cpPinJointNew(body,
                                                  ship2_tiles.at(x, y - 1).getBody(),
                                                  cpv(0.0, -TILE_DIAMETER / 2.0),
                                                  cpv(0.0, +TILE_DIAMETER / 2.0));
                cpConstraintSetErrorBias(con, error_bias);
                cpConstraintSetMaxForce(con, MAX_JOINT_FORCE);
                cpConstraintSetCollideBodies(con, cpTrue);
                cpSpaceAddConstraint(space, con);
                ship2_tiles.at(x, y).setAboveConstraint(con);
            }
        }
    }

}

void CleanUp() {
    for (int y = 0; y < SHIP1_HEIGHT; y += 1) {
        for (int x = 0; x < SHIP1_WIDTH; x += 1) {
            ship1_tiles.at(x, y).destroy();
        }
    }

    for (int y = 0; y < SHIP2_HEIGHT; y += 1) {
        for (int x = 0; x < SHIP2_WIDTH; x += 1) {
            ship2_tiles.at(x, y).destroy();
        }
    }
    cpSpaceFree(space);
    space = nullptr;
}

void Update() {
    cpSpaceStep(space, 1.0 / 30.0);
    for (int y = 0; y < SHIP1_HEIGHT; y += 1) {
        for (int x = 0; x < SHIP1_WIDTH; x += 1) {
            ship1_tiles.at(x, y).update();
        }
    }

    for (int y = 0; y < SHIP2_HEIGHT; y += 1) {
        for (int x = 0; x < SHIP2_WIDTH; x += 1) {
            ship2_tiles.at(x, y).update();
        }
    }
}

void Draw(sf::RenderTarget& target) {
    sf::CircleShape cir{TILE_DIAMETER / 2.0};

    // Ship 1
    cir.setFillColor(sf::Color::Green);
    for (int y = 0; y < SHIP1_HEIGHT; y += 1) {
        for (int x = 0; x < SHIP1_WIDTH; x += 1) {
            const ShipTile& tile = ship1_tiles.at(x, y);
            const cpBody* body = tile.getBody();
            const cpVect pos = cpBodyGetPosition(body);
            cir.setPosition({float(pos.x), float(pos.y)});
            target.draw(cir);
        }
    }

    // Ship 2
    cir.setFillColor(sf::Color::Red);
    for (int y = 0; y < SHIP2_HEIGHT; y += 1) {
        for (int x = 0; x < SHIP2_WIDTH; x += 1) {
            const ShipTile& tile = ship2_tiles.at(x, y);
            const cpBody* body = tile.getBody();
            const cpVect pos = cpBodyGetPosition(body);
            cir.setPosition({float(pos.x), float(pos.y)});
            target.draw(cir);
        }
    }
}

void MoveView(sf::View& view) {
    const float offset = 5.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        view.move({-offset, 0.0f});
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        view.move({+offset, 0.0f});
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        view.move({0.0f, -offset});
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        view.move({0.0f, +offset});
    }
}

int main() {

    Initialize();

    // Drawing stuff
    sf::RenderWindow window(sf::VideoMode(800, 800), "Spaceship physics");
    //sf::View view{{75, 75}, {150, 150}};
    sf::View view{{50, 50}, {100, 100}};
    window.setView(view);

    sf::RectangleShape redDot;
    redDot.setSize({1, 1});
    redDot.setFillColor(sf::Color::Red);

    window.setFramerateLimit(30u);

    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        sf::Vector2f mouseCoord = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        mouseCoord.x = round(mouseCoord.x);
        mouseCoord.y = round(mouseCoord.y);
        redDot.setPosition(mouseCoord);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            for (int y = 0; y < SHIP2_HEIGHT; y += 1) {
                const ShipTile& tile = ship2_tiles.at(SHIP1_WIDTH - 1, y);
                cpBody* body = tile.getBody();
                cpBodyApplyForceAtLocalPoint(body, cpv(-400.0, 0.0), cpv(0.0, 0.0));
            }
        }
            

        Update();
        MoveView(view);
        window.setView(view);

        window.clear();
        Draw(window);
        window.draw(redDot);
        window.display();
    }

    CleanUp();

    return 0;
}

#if 0
int main() {
    // cpVect is a 2D vector and cpv() is a shortcut for initializing them.
    cpVect gravity = cpv(0, -100);
  
    // Create an empty space.
    cpSpace *space = cpSpaceNew();
    cpSpaceSetGravity(space, gravity);
  
    // Add a static line segment shape for the ground.
    // We'll make it slightly tilted so the ball will roll off.
    // We attach it to a static body to tell Chipmunk it shouldn't be movable.
    cpShape *ground = cpSegmentShapeNew(cpSpaceGetStaticBody(space), cpv(-20, 5), cpv(20, -5), 0);
    cpShapeSetFriction(ground, 1);
    cpSpaceAddShape(space, ground);
  
    // Now let's make a ball that falls onto the line and rolls off.
    // First we need to make a cpBody to hold the physical properties of the object.
    // These include the mass, position, velocity, angle, etc. of the object.
    // Then we attach collision shapes to the cpBody to give it a size and shape.
  
    cpFloat radius = 5;
    cpFloat mass = 1;
  
    // The moment of inertia is like mass for rotation
    // Use the cpMomentFor*() functions to help you approximate it.
    cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);
  
    // The cpSpaceAdd*() functions return the thing that you are adding.
    // It's convenient to create and add an object in one line.
    cpBody *ballBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
    cpBodySetPosition(ballBody, cpv(0, 15));
  
    // Now we create the collision shape for the ball.
    // You can create multiple collision shapes that point to the same body.
    // They will all be attached to the body and move around to follow it.
    cpShape *ballShape = cpSpaceAddShape(space, cpCircleShapeNew(ballBody, radius, cpvzero));
    cpShapeSetFriction(ballShape, 0.7);
  
    // Now that it's all set up, we simulate all the objects in the space by
    // stepping forward through time in small increments called steps.
    // It is *highly* recommended to use a fixed size time step.
    cpFloat timeStep = 1.0/60.0;
    for(cpFloat time = 0; time < 2; time += timeStep){
        cpVect pos = cpBodyGetPosition(ballBody);
        cpVect vel = cpBodyGetVelocity(ballBody);
        printf(
            "Time is %5.2f. ballBody is at (%5.2f, %5.2f). It's velocity is (%5.2f, %5.2f)\n",
            time, pos.x, pos.y, vel.x, vel.y
        );
    
        cpSpaceStep(space, timeStep);
    }
  
    // Clean up our objects and exit!
    cpSpaceRemoveShape(space, ballShape);
    cpShapeFree(ballShape); 

    cpSpaceRemoveShape(space, ground);
    cpShapeFree(ground);

    cpSpaceRemoveBody(space, ballBody);
    cpBodyFree(ballBody);

    cpSpaceFree(space);
  
    return 0;
}
#endif