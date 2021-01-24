
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Math.hpp>
#include <SFML/Graphics.hpp>

#include <cassert>
#include <cmath>
#include <cstring>
#include <limits>
#include <memory>
#include <vector>

#include "Camera.hpp"
#include "Matrix.hpp"
#include "Vector.hpp"

using namespace jbatnozic::raynor;
using hg::PZInteger;

// Solves a quadratic equation
// aX0 will contain the lower solution, aX1 will contain the larger solution (if any)
// Returns false if can't be solved in real numbers
template <class taReal>
bool SolveQuadratic(taReal aA, taReal aB, taReal aC, taReal& aX0, taReal& aX1)
{
    using Real = taReal;

    const Real discriminant = (aB * aB) - (Real{4} * aA * aC);
    if (discriminant < Real{0}) {
        return false;
    }

    if (discriminant == Real{0}) {
        aX0 = aX1 = Real{-0.5} * aB / aA;
    }
    else {
        const Real q = (aB > Real{0}) ? -0.5 * (aB + std::sqrt(discriminant))
                                      : -0.5 * (aB - std::sqrt(discriminant));
        aX0 = q / aA;
        aX1 = aC / q;
    }

    if (aX0 > aX1) {
        std::swap(aX0, aX1);
    }

    return true;
}

class Light {
public:
    Light() {}
};

template <class taReal>
class Object {
public:
    using Real = taReal;

    Object() {}
    virtual ~Object() {}

    virtual bool intersects(const Vec3<Real>& aOrigin, const Vec3<Real>& aDirection, Real& t) const = 0;

    virtual sf::Color getColor() const = 0;
};

template <class taReal>
class Sphere : public Object<taReal> {
public:
    using Real = taReal;

    Sphere(Vec3<Real> aCentre, Real aRadius, sf::Color color)
        : _centre{aCentre}
        , _radius{aRadius}
        , _radius2{aRadius * aRadius}
        , _color{color}
    {
    }

    bool intersects(const Vec3<Real>& aOrigin, const Vec3<Real>& aDirection, Real& t) const override {
        Real t0, t1; // solutions for t if the ray intersects 

        const Vec3<Real> L = aOrigin - _centre;
        const Real a = Dot(aDirection, aDirection);
        const Real b = 2 * Dot(aDirection, L);
        const Real c = Dot(L, L) - _radius2;
        if (!SolveQuadratic(a, b, c, t0, t1)) {
            return false;
        }

        if (t0 > t1) {
            std::swap(t0, t1);
        }

        if (t0 < 0) {
            t0 = t1; // if t0 is negative, let's use t1 instead 
            if (t0 < 0) return false; // both t0 and t1 are negative 
        }

        t = t0;

        return true;
    }

    sf::Color getColor() const override {
        return _color;
    }

private:
    Vec3<Real> _centre;
    Real _radius;
    Real _radius2;
    sf::Color _color;
};

template <class taReal>
sf::Color CastRay(
    const Vec3<taReal>& aOrigin,
    const Vec3<taReal>& aDirection,
    const std::vector<std::unique_ptr<Object<taReal>>>& aObjects,
    const std::vector<std::unique_ptr<Light>>& aLights,
    PZInteger aMaxDepth)
{
    sf::Color result = sf::Color::Black;

    // result.r = static_cast<sf::Uint8>(((aDirection + Vec3<taReal>{taReal{1.0}}) * 0.5).x * taReal{255.0});
    // result.g = static_cast<sf::Uint8>(((aDirection + Vec3<taReal>{taReal{1.0}}) * 0.5).y * taReal{255.0});
    // result.b = static_cast<sf::Uint8>(((aDirection + Vec3<taReal>{taReal{1.0}}) * 0.5).z * taReal{255.0});

    taReal minDistance = std::numeric_limits<taReal>::infinity();

    for (const auto& object : aObjects) {
        taReal distance;
        if (object->intersects(aOrigin, aDirection, distance)) {
            if (distance < minDistance) {
                result = object->getColor();
                minDistance = distance;
            }
        }
    }

    return result;
}

template <class taReal>
void Render(sf::Uint32* aPixels, 
            PZInteger aResolutionWidth,
            PZInteger aResolutionHeight,
            hg::math::Angle aFieldOfView,
            const std::vector<std::unique_ptr<Object<taReal>>>& aObjects,
            const std::vector<std::unique_ptr<Light>>& aLights) {
    // Make camera
    auto cameraAtOrigin = ConstructCameraAtOrigin<taReal>(aResolutionWidth, aResolutionHeight, aFieldOfView);

    // Transform camera
    // TODO

    // Cast 
    for (PZInteger y = 0; y < aResolutionHeight; y += 1) {
        for (PZInteger x = 0; x < aResolutionWidth; x += 1) {
            sf::Uint32* pixel32 = aPixels + (y * aResolutionWidth + x);
            sf::Uint8* pixel = (sf::Uint8*)pixel32;

            if (x == 0 || y == 0 || x == aResolutionWidth - 1 || y == aResolutionHeight - 1) {
                // BORDER
                pixel[0] = 0xFF; // r
                pixel[1] = 0x00; // g
                pixel[2] = 0x00; // b
                pixel[3] = 0xFF; // a
            }
            else {
                // INTERIOR
                auto colour = CastRay(Vec3<taReal>{0.0},
                                      cameraAtOrigin[y * aResolutionWidth + x],
                                      aObjects,
                                      aLights,
                                      4);

                pixel[0] = colour.r; // r
                pixel[1] = colour.g; // g
                pixel[2] = colour.b; // b
                pixel[3] = 0xFF; // a
            }

        }
    }

}

void Display(const sf::Sprite& sprite) {
    sf::RenderWindow window(sf::VideoMode(800, 800), "RAYNOR");
    window.setFramerateLimit(30u);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
}

#define REAL double

void TestQuadratic() {
    double t0, t1;
    bool success;

    success = SolveQuadratic<double>(1, 4, 3, t0, t1);
    assert(success);
    assert(std::abs(t0 - (-3.0)) < 0.01);
    assert(std::abs(t1 - (-1.0)) < 0.01);

    success = SolveQuadratic<double>(1, 2, 3, t0, t1);
    assert(!success);

    success = SolveQuadratic<double>(10, 20, -50, t0, t1);
    assert(success);
    assert(std::abs(t0 - (-3.449)) < 0.01);
    assert(std::abs(t1 - (1.449)) < 0.01);

    success = SolveQuadratic<double>(10, 20, 50, t0, t1);
    assert(!success);
}

void Test() {
    Sphere<REAL> sphere{Vec3<REAL>{2, 0, 0}, 1, sf::Color::White};

    const auto ray1 = Vec3<REAL>{1, 0, 0}; // X-axis
    const auto ray2 = Vec3<REAL>{0, 1, 0}; // Y-axis
    const auto ray3 = Normalize(Vec3<REAL>{1, 1, 0}); // Diagonal between X and Y axes
    const auto ray4 = Normalize(Vec3<REAL>{ // Barely in the circle
        std::cos(hg::math::Angle::fromDegrees(29).asRadians()),
        std::sin(hg::math::Angle::fromDegrees(29).asRadians()),
        0
    }); 
    const auto ray5 = Normalize(Vec3<REAL>{ // Barely outside the circle
        std::cos(hg::math::Angle::fromDegrees(31).asRadians()),
        std::sin(hg::math::Angle::fromDegrees(31).asRadians()),
        0
    });

    constexpr Vec3<REAL> ORIGIN = Vec3<REAL>{0};
    REAL t;
    assert(sphere.intersects(ORIGIN, ray1, t) == true);
    assert(sphere.intersects(ORIGIN, ray2, t) == false);
    assert(sphere.intersects(ORIGIN, ray3, t) == false);
    assert(sphere.intersects(ORIGIN, ray4, t) == true);
    assert(sphere.intersects(ORIGIN, ray5, t) == false);

}


#define RESOLUTION_WIDTH  800
#define RESOLUTION_HEIGHT 800

int main() {
    TestQuadratic();
    Test();

    // Set the scene:
    std::vector<std::unique_ptr<Object<REAL>>> objects;
    std::vector<std::unique_ptr<Light>> lights;

    objects.push_back(std::make_unique<Sphere<REAL>>(
        Vec3<REAL>{1, -20, 0},
        2,
        sf::Color::Yellow
        ));

    objects.push_back(std::make_unique<Sphere<REAL>>(
        Vec3<REAL>{-1, -20, 0},
        2,
        sf::Color::Red
        ));

    objects.push_back(std::make_unique<Sphere<REAL>>(
        Vec3<REAL>{0, -19, 1},
        2,
        sf::Color::Green
        ));

    // Render:
    sf::Uint32* bitmap = new sf::Uint32[RESOLUTION_WIDTH * RESOLUTION_HEIGHT];

    Render<REAL>(bitmap,
                 RESOLUTION_WIDTH,
                 RESOLUTION_HEIGHT,
                 hg::math::Angle::fromDegrees(90), 
                 objects, 
                 lights);

    sf::Texture texture;
    texture.create(RESOLUTION_WIDTH, RESOLUTION_HEIGHT);
    texture.update((sf::Uint8*)bitmap, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0);
    sf::Sprite sprite(texture);

    Display(sprite);

    delete[] bitmap;

    return 0;
}