
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Time_utils.hpp>
#include <SFML/Graphics.hpp>

#include <cassert>
#include <cmath>
#include <cstring>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

#include <Raynor/Renderer.hpp>

namespace ray = jbatnozic::raynor;

void Display(std::function<void(sf::Texture& aTexture, sf::Sprite& sprite)> aRender) {
    sf::RenderWindow window(sf::VideoMode(800, 800), "RAYNOR");
    window.setFramerateLimit(30u);

    sf::Texture texture;
    sf::Sprite sprite;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        aRender(texture, sprite);
        window.clear();
        window.draw(sprite);
        window.display();
    }
}

#define REAL double

void TestQuadratic() {
    double t0, t1;
    bool success;

    success = hg::math::SolveQuadratic<double>(1, 4, 3, t0, t1);
    assert(success);
    assert(std::abs(t0 - (-3.0)) < 0.01);
    assert(std::abs(t1 - (-1.0)) < 0.01);

    success = hg::math::SolveQuadratic<double>(1, 2, 3, t0, t1);
    assert(!success);

    success = hg::math::SolveQuadratic<double>(10, 20, -50, t0, t1);
    assert(success);
    assert(std::abs(t0 - (-3.449)) < 0.01);
    assert(std::abs(t1 - (1.449)) < 0.01);

    success = hg::math::SolveQuadratic<double>(10, 20, 50, t0, t1);
    assert(!success);
}

//void TestIntersect() {
//    Sphere<REAL> sphere{Vec3<REAL>{2, 0, 0}, 1, sf::Color::White};
//
//    const auto ray1 = Vec3<REAL>{1, 0, 0}; // X-axis
//    const auto ray2 = Vec3<REAL>{0, 1, 0}; // Y-axis
//    const auto ray3 = Normalize(Vec3<REAL>{1, 1, 0}); // Diagonal between X and Y axes
//    const auto ray4 = Normalize(Vec3<REAL>{ // Barely in the circle
//        std::cos(hg::math::Angle<REAL>::fromDegrees(29).asRadians()),
//        std::sin(hg::math::Angle<REAL>::fromDegrees(29).asRadians()),
//        0
//    }); 
//    const auto ray5 = Normalize(Vec3<REAL>{ // Barely outside the circle
//        std::cos(hg::math::Angle<REAL>::fromDegrees(31).asRadians()),
//        std::sin(hg::math::Angle<REAL>::fromDegrees(31).asRadians()),
//        0
//    });
//
//    constexpr Vec3<REAL> ORIGIN = Vec3<REAL>{0};
//    REAL t;
//    assert(sphere.intersects(ORIGIN, ray1, t) == true);
//    assert(sphere.intersects(ORIGIN, ray2, t) == false);
//    assert(sphere.intersects(ORIGIN, ray3, t) == false);
//    assert(sphere.intersects(ORIGIN, ray4, t) == true);
//    assert(sphere.intersects(ORIGIN, ray5, t) == false);
//}

#define RESOLUTION_WIDTH  400
#define RESOLUTION_HEIGHT 400

int main() {
    TestQuadratic();
    // TestIntersect();

    ray::Renderer<REAL> renderer{RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 1};

    // Set the scene:
    ray::Sphere<REAL> sphere1{ray::Vec3<REAL>{1, -20, 0}, 2, sf::Color::Yellow};
    ray::Sphere<REAL> sphere2{ray::Vec3<REAL>{-1, -20, 0}, 2, sf::Color::Red};
    ray::Sphere<REAL> sphere3{ray::Vec3<REAL>{0, -19, 1}, 2, sf::Color::Green};

    auto h1 = renderer.addObject(&sphere1);
    auto h2 = renderer.addObject(&sphere2);
    auto h3 = renderer.addObject(&sphere3);

    // Render:
    REAL cameraAngle = 0.f;
    REAL cameraElevation = 0.f;
    REAL cameraX = 0.f;
    REAL cameraY = 0.f;

    Display(
        [&](sf::Texture& aTexture, sf::Sprite& aSprite) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                cameraAngle += 0.02;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                cameraAngle -= 0.02;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                cameraElevation += 0.02;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                cameraElevation -= 0.02;
            }

            cameraX += (REAL)sf::Keyboard::isKeyPressed(sf::Keyboard::Right) -
                (REAL)sf::Keyboard::isKeyPressed(sf::Keyboard::Left);

            cameraY += (REAL)sf::Keyboard::isKeyPressed(sf::Keyboard::Down) -
                (REAL)sf::Keyboard::isKeyPressed(sf::Keyboard::Up);

            ray::RenderSettings<REAL> settings;
            settings.canvasWidth = RESOLUTION_WIDTH;
            settings.canvasHeight = RESOLUTION_HEIGHT;
            settings.cameraOrigin = ray::Vec3<REAL>{cameraX, cameraY, 0.f};
            settings.cameraAngle = hg::math::Angle<REAL>::fromRadians(cameraAngle);
            settings.cameraElevation = hg::math::Angle<REAL>::fromRadians(cameraElevation);
            settings.fieldOfView = hg::math::Angle<REAL>::fromDegrees(90.f);

            hg::util::Stopwatch stopwatch;
            renderer.render(settings);
            printf("%dms\n", (int)stopwatch.getElapsedTime().count());

            if (aTexture.getSize().x != RESOLUTION_WIDTH &&
                aTexture.getSize().y != RESOLUTION_HEIGHT) {
                aTexture.create(RESOLUTION_WIDTH, RESOLUTION_HEIGHT);
            }
            aTexture.update(renderer.getBitmap(), RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0);
            aSprite = sf::Sprite{aTexture};
            aSprite.setScale({2.f, 2.f});
        });

    return 0;
}