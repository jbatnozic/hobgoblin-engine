
#include <Hobgoblin/QAO.hpp>

#include <cstdio>
#include <iostream>
#include <memory>
#include <vector>

#include <chipmunk\chipmunk.h>
#include <SFML\Graphics.hpp>

#include "Ship.hpp"

constexpr cpFloat TILE_DIAMETER = 4.0;
constexpr cpFloat MAX_JOINT_FORCE = 1000.0;

namespace hg = jbatnozic::hobgoblin;

class Ship2 : public hg::QAO_Base {
public:
    Ship2()
        : hg::QAO_Base(0)
    {
    }
};

namespace {
    cpSpace* space;
    std::unique_ptr<Ship> ship1;
    std::unique_ptr<Ship> ship2;
}

void Initialize() {
    Ship2 _ship2;

    // Create space
    space = cpSpaceNew();
    //cpSpaceUseSpatialHash(space, TILE_DIAMETER * 2.0, SHIP_WIDTH * SHIP_HEIGHT * 10); // Experimental

    // Create ships
    ShipSpec spec;

    spec.height = 20;
    spec.width = 10;
    spec.max_joint_force = 2.25 * MAX_JOINT_FORCE;
    spec.tile_mass = 10.0;
    spec.tile_size = TILE_DIAMETER;
    spec.init_x = 10.0;
    spec.init_y = 10.0;
    spec.init_angle = DegToRad(Degrees{0});
    ship1 = std::make_unique<Ship>(space, spec);

    spec.height = 20;
    spec.width = 5;
    spec.max_joint_force = MAX_JOINT_FORCE;
    spec.tile_mass = 1.0;
    spec.tile_size = TILE_DIAMETER;
    spec.init_x = 250;
    spec.init_y = 10.0;
    spec.init_angle = DegToRad(Degrees{90});
    ship2 = std::make_unique<Ship>(space, spec);
}

void CleanUp() {
    ship1.reset();
    ship2.reset();

    cpSpaceFree(space);
    space = nullptr;
}

void Update(cpVect mouseCoords) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
        ship1->power(mouseCoords, 5000.0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
        ship2->power(mouseCoords, 30000.0);
    }

    cpSpaceStep(space, 1.0 / 30.0); // TODO Extract framerate
    ship1->update();
    ship2->update();
}

void Draw(sf::RenderTarget& target) {
    ship1->drawSelf(target, sf::Color::Green);
    ship2->drawSelf(target, sf::Color::Red);
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
    sf::View view{{100, 100}, {200, 200}};
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

        sf::Vector2f mouseCoords = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        redDot.setPosition(mouseCoords);

        #if 0
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            const ShipTile& tile = ship1.at(0, 0);
            cpBody* body = tile.getBody();
            cpVect pos = cpBodyGetPosition(body);
            cpVect force = cpvclamp(cpv(mouseCoord.x - pos.x, mouseCoord.y - pos.y), 5.0);
           
            /*cpBodySetAngle(body, cpBodyGetAngle(body) + 0.01);
            cpBodyApplyForceAtLocalPoint(body, cpv(1, 0), cpv(0, 0));*/
            cpBodyApplyForceAtWorldPoint(body, force, pos);
            std::cout << "Angle = " << cpBodyGetAngle(body) << "\n";
            //(body, force, CP_ZERO_VECTOR);
        }    
        #endif

        Update(cpv(mouseCoords.x, mouseCoords.y));
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
