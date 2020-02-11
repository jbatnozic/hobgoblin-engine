#if 0

#include <SFML/Graphics.hpp>

#include <cmath>
#include <vector>

constexpr double MATH_PI = 3.14159265358979323846;

template <typename T>
T Sqr(const T arg) {
    return arg * arg;
}

double CrossProduct2D(sf::Vector2<double> a, sf::Vector2<double> b) {
    return a.x * b.y - a.y * b.x;
}

constexpr double DegToRad(double deg) {
    constexpr double pi = 3.1415926535;
    return (deg * pi / 180.0);
}

struct Radians {
    double rad;
    Radians() = default;
    constexpr explicit Radians(double rad) : rad(rad) {}
};

struct Degrees {
    double deg;
    Degrees() = default;
    constexpr explicit Degrees(double deg) : deg(deg) {}
};

Radians DegToRad(Degrees deg) {
    return Radians{deg.deg * MATH_PI / 180.0};
}

Degrees RadToDeg(Radians rad) {
    return Degrees{rad.rad * 180 / MATH_PI};
}

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

struct ShipPixel {
    double speed;
    double direction;
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
    std::vector<int> points;
    std::vector<int> right_angle_projection;

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

    int* rightAngleProjectionAt(int xx, int yy) {
        const int idx = yy * rap_side + xx;
        if (idx < 0 || idx > right_angle_projection.size()) {
            return nullptr;
        }
        return &right_angle_projection[idx];
    }

    void renderSelf() {
        std::memset(&right_angle_projection[0], 0x00, right_angle_projection.size() * sizeof(int));

        for (int i = 0; i < width; i += 1) {
            for (int t = 0; t < height; t += 1) {
                double xoff = (double)i - width / 2;
                double yoff = (double)t - height / 2;

                double dist = sqrt(Sqr(xoff) + Sqr(yoff));
                double theta = atan2(yoff, xoff) + image_angle;

                int xx = static_cast<int>(dist * cos(theta)) + rap_side / 2;
                int yy = static_cast<int>(dist * sin(theta)) + rap_side / 2;

                int* p = rightAngleProjectionAt(xx, yy);
                if (p) *p = 1;
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
                if (*rightAngleProjectionAt(i, t) == 0) continue;
                double xx = x + (double)i - rap_side / 2;
                double yy = y + (double)t - rap_side / 2;
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
        moment += CrossProduct2D({force.getX() / mass, force.getY() / mass}, {xoff, yoff}); // TODO ???
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

int main() {
    // Ship stuff
    Ship ship1{100, 100};
    ship1.x = 75.0;
    ship1.y = 75.0;
    ship1.applyForce(50, -50, PhysicsVector2D{0.0, Degrees{135.0}});
    ship1.moment = 0.001;

    // Drawing stuff
    sf::RenderWindow window(sf::VideoMode(800, 800), "Spaceship physics");
    sf::View view{{75, 75}, {150, 150}};
    window.setView(view);

    window.setFramerateLimit(30u);

    while (window.isOpen()) {

        ship1.update();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        ship1.drawSelf(window);
        window.display();
    }

    return 0;
}

#endif