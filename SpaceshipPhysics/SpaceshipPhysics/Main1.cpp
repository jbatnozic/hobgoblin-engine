#include <SFML/Graphics.hpp>

#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>
#include <deque>
#include <random>

#include "RandomGen.hpp"
#include "RowMajorMatrix.hpp"
#include "PhysicsCommon.hpp"

// TODO double/float -> Real

sf::Font font;

class PhysicsVector2D {
public:
    PhysicsVector2D() = default;

    PhysicsVector2D(double x, double y)
        : x(x), y(y)
    {
        adjustPolar();
    }

    PhysicsVector2D(double intensity, Radians direction)
        : intensity(intensity), direction(direction)
    {
        adjustCartesian();
    }

    PhysicsVector2D(double intensity, Degrees direction)
        : intensity(intensity), direction(DegToRad(direction))
    {
        adjustCartesian();
    }

    double getX() const {
        return x;
    }

    double getY() const {
        return y;
    }

    double getIntensity() const {
        return intensity;
    }

    Radians getDirectionRad() const {
        return direction;
    }

    Degrees getDirectionDeg() const {
        return RadToDeg(direction);
    }

    void setCartesian(double x_, double y_) {
        x = x_;
        y = y_;
        adjustPolar();
    } 

    void setPolar(double intensity_, Radians direction_) {
        intensity = intensity_;
        direction = direction_;
        adjustCartesian();
    }

    void setPolar(double intensity_, Degrees direction_) {
        intensity = intensity_;
        direction = DegToRad(direction_);
        adjustCartesian();
    }

    PhysicsVector2D operator+(const PhysicsVector2D& rhs) {
        return PhysicsVector2D(x + rhs.x, y + rhs.y);
    }

    PhysicsVector2D operator-(const PhysicsVector2D& rhs) {
        return PhysicsVector2D(x - rhs.x, y - rhs.y);
    }

    PhysicsVector2D operator-() {
        return PhysicsVector2D(-x, -y);
    }

private:
    double x;
    double y;
    double intensity;
    Radians direction;

    void adjustCartesian() {
        x = +intensity * cos(direction.rad);
        y = -intensity * sin(direction.rad);
    }

    void adjustPolar() {
        intensity = sqrt(Sqr(x) + Sqr(y));
        direction = Radians{atan2(y, x)};
    }
};

///////////////////////////////////////////////////////////////////////////////

struct ShipPixel {
    sf::Color color;
    double speed;
    double direction;
    ShipPixel* parent;
};

struct Ship {
    int width;
    int height;
    double x;
    double y;
    PhysicsVector2D speed;
    double moment;
    double image_angle;
    int rap_side;
    std::vector<ShipPixel> points;
    std::vector<ShipPixel> right_angle_projection;

    Ship(int width, int height)
        : width{width}
        , height{height}
        , x{}, y{}, speed{}, moment{}
        , image_angle{}
        , rap_side{(int) ceil(2.0 * sqrt(Sqr(width / 2) + Sqr(height / 2)))}
        , points{}
        , right_angle_projection{}
    {
        points.resize(width * height);
        right_angle_projection.resize(Sqr(rap_side));
    }

    void update() {
        image_angle -= moment;
        x += speed.getX();
        y += speed.getY();
    }

    ShipPixel* rightAngleProjectionAt(int xx, int yy) {
        const int idx = yy * rap_side + xx;
        if (idx < 0 || idx > right_angle_projection.size()) {
            return nullptr;
        }
        return &right_angle_projection[idx];
    }

    void renderSelf() {
        std::memset(&right_angle_projection[0], 0x00, right_angle_projection.size() * sizeof(ShipPixel)); // Dangerous

        for (int i = 0; i < width; i += 1) {
            for (int t = 0; t < height; t += 1) {
                double xoff = (double)i - width / 2;
                double yoff = (double)t - height / 2;

                double dist = sqrt(Sqr(xoff) + Sqr(yoff));
                double theta = atan2(yoff, xoff) + image_angle;

                int xx = static_cast<int>(dist * cos(theta)) + rap_side / 2;
                int yy = static_cast<int>(dist * sin(theta)) + rap_side / 2;

                ShipPixel* p = rightAngleProjectionAt(xx, yy);
                if (p) p->parent = &points[t * width + i];
            }
        }
    }

    void drawSelf(sf::RenderTarget& target) {
        renderSelf();

        sf::RectangleShape rect;
        rect.setFillColor(sf::Color::Green);
        rect.setSize({1, 1});
        for (int i = 0; i < rap_side; i += 1) {
            for (int t = 0; t < rap_side; t += 1) {
                const ShipPixel* spix = rightAngleProjectionAt(i, t);
                if (spix->parent == nullptr) continue;
                double xx = round(x + (double)i - rap_side / 2);
                double yy = round(y + (double)t - rap_side / 2);
                rect.setPosition(xx, yy);
                target.draw(rect);
            }
        }
    }

    void applyForce(double xoff, double yoff, PhysicsVector2D force) {
        const double mass = 1.0;
        // Speed/direction
        const double xspeed_new = speed.getX() + (force.getX() / mass);
        const double yspeed_new = speed.getY() + (force.getY() / mass);
        speed.setCartesian(xspeed_new, yspeed_new);
        // Moment/rotation
        const double m = CrossProduct2D({force.getX() / mass, force.getY() / mass}, {xoff, yoff});
        moment += m / sqrt(Sqr(xoff) + Sqr(yoff));
    }

    void collide(Ship& other) {
        const int my_x = static_cast<int>(x) - rap_side / 2;
        const int my_y = static_cast<int>(y) - rap_side / 2;
        const int ot_x = static_cast<int>(other.x) - rap_side / 2;
        const int ot_y = static_cast<int>(other.y) - rap_side / 2;

        if (my_x + rap_side < ot_x || my_x >= ot_x + other.rap_side ||
            my_y + rap_side < ot_y || my_y >= ot_y + other.rap_side) {
            return;
        }
    }

};

///////////////////////////////////////////////////////////////////////////////

#include "StrainCalc.hpp"

///////////////////////////////////////////////////////////////////////////////

//#define SHIP_TEST

int arrangement[12][12] =
{ { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
, { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 }
, { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 }
, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

static const int SIZE_TEST = sizeof(StrainCalc);

using namespace std::chrono;

int main() {
    font.loadFromFile("U:\\SYNCHRONIZED\\TABLETOP\\BSG\\Fonts\\Eurostile.ttf");
    //GeneralPurposeRandomEngine().seed(SystemRandomSeed<unsigned>());
    GeneralPurposeRandomEngine().seed(12345u);
    FastRandomEngine().seed(12345u);
#ifdef SHIP_TEST
    // Ship stuff
    Ship ship1{100, 100};
    ship1.x = 75.0;
    ship1.y = 75.0;
    ship1.applyForce(50, -50, PhysicsVector2D{0.0, Degrees{135.0}});
    ship1.moment = 0.001;
#endif

    #define SIDE 64
    StrainCalc sc{SIDE};

    /*while (true) {

        Tile tile1, tile2;
        tile1.force_y = -1000.0;
        tile1.strain = 0.0;
        tile1.empty = false;
        tile2.force_y = +1000.0;
        tile2.strain = 0.0;
        tile2.empty = false;

        //const auto t1 = high_resolution_clock::now();
        Real temp;
        for (int i = 0; i < 200'000; i += 1) {
            tile1.force_y += (int)FastRandomEngine()() % 100 - 50;
            tile2.force_y += (int)FastRandomEngine()() % 100 - 50;
            temp = StrainCalc::equilizeNeightbors(tile1, tile2);
        }
        /*const auto t2 = high_resolution_clock::now();
        std::cout << "Operation took " 
                    << static_cast<double>(duration_cast<microseconds>(t2 - t1).count()) / 1000.0 
                    << " millliseconds and temp = " << temp << ".\n";

    }*/

    for (int cnt = 0; cnt < 20; cnt += 1)
    //while (true)
    {

        for (int y = 1; y <= SIDE; y += 1) {
            for (int x = 1; x <= SIDE; x += 1) {
                sc.matrix.at(x, y).empty = !(x < SIDE / 2);//!arrangement[y - 1][x - 1];
            }
        }
        sc.setAllForces(0, 10); // total: 1240 downward
        for (int i = 1; i <= SIDE; i += 1) {
            sc.matrix.at(i, SIDE).force_y += -SIDE * 10;//-103.333;
        }
        sc.createEquilizationOrder();

        const auto t1 = high_resolution_clock::now();
        const int iter = sc.equilize();
        const auto t2 = high_resolution_clock::now();
        std::cout << "Operation took " 
                  << static_cast<double>(duration_cast<microseconds>(t2 - t1).count()) / 1000.0 
                  << " millliseconds and " << iter << " iterations.\n";

    }

    // Drawing stuff
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Spaceship physics");
    //sf::View view{{75, 75}, {150, 150}};
    sf::View view{{500, 500}, {1000, 1000}};
    window.setView(view);

    sf::RectangleShape redDot;
    redDot.setSize({1, 1});
    redDot.setFillColor(sf::Color::Red);

    window.setFramerateLimit(30u);

    while (window.isOpen()) {

    #ifdef SHIP_TEST
        ship1.update();
    #endif

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                const auto t1 = high_resolution_clock::now();
                const int iter = sc.equilize();
                const auto t2 = high_resolution_clock::now();
                std::cout << "Operation took " 
                            << static_cast<double>(duration_cast<microseconds>(t2 - t1).count()) / 1000.0 
                            << " millliseconds and " << iter << " iterations.\n";
            }
        }

        sf::Vector2f mouseCoord = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        mouseCoord.x = round(mouseCoord.x);
        mouseCoord.y = round(mouseCoord.y);
        redDot.setPosition(mouseCoord);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        }

        window.clear();
    #ifdef SHIP_TEST
        ship1.drawSelf(window);
    #endif
        sc.drawSelf(window);
        window.draw(redDot);
        window.display();
    }

    return 0;
}
