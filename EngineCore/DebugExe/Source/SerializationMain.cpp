
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Utility/Serialization.hpp>
#include <Hobgoblin/Preprocessor.hpp>

#include <iostream>
#include <memory>

using namespace hg::util;

// hpp:

struct Point {
    static constexpr auto SERIALIZABLE_TAG = "Point";

    int x;
    int y;

    Point(int xx, int yy) : x{xx}, y{yy} {}

    void serialize(Packet& packet) const { // TODO Rename to rawSerialize/serializeImpl or something...
        packet << x << y;
    }

    static void deserialize(Packet& packet, AnyPtr context, int /* contextTag */) {
        std::vector<Point>& points = *context.getOrThrow<std::vector<Point>>();
        int x = packet.extract<int>();
        int y = packet.extract<int>();
        points.emplace_back(x, y);
    }
};

struct Shape : PolymorphicSerializable {
    virtual ~Shape() = 0 {}
    virtual double area() const = 0;
};

struct Circle : Shape {
    static constexpr auto SERIALIZABLE_TAG = "Circle";
    
    double r;

    Circle(double rr) : r{rr} {}

    void serialize(Packet& packet) const override {
        packet << r;
    }

    std::string getSerializableTag() const {
        return SERIALIZABLE_TAG;
    }

    static void deserialize(Packet& packet, AnyPtr context, int /* contextTag */) {
        auto& shapes = *context.getOrThrow<std::vector<std::unique_ptr<Shape>>>();
        double r = packet.extract<double>();
        shapes.emplace_back(std::make_unique<Circle>(r));
    }

    double area() const override {
        return r * r * 3.1415926535;
    }
};

struct Rectangle : Shape {
    static constexpr auto SERIALIZABLE_TAG = "Rectangle";

    double x, y;

    Rectangle(double xx, double yy) : x{xx}, y{yy} {}

    void serialize(Packet& packet) const override {
        packet << x << y;
    }

    std::string getSerializableTag() const {
        return SERIALIZABLE_TAG;
    }

    static void deserialize(Packet& packet, AnyPtr context, int /* contextTag */) {
        auto& shapes = *context.getOrThrow<std::vector<std::unique_ptr<Shape>>>();
        double x = packet.extract<double>();
        double y = packet.extract<double>();
        shapes.emplace_back(std::make_unique<Rectangle>(x, y));
    }

    double area() const override {
        return x * y;
    }
};

// cpp:

HG_PP_DO_BEFORE_MAIN(RegisterSerializables) {
    RegisterSerializable<Point>();
    RegisterSerializable<Circle>();
    RegisterSerializable<Rectangle>();
}

int main() {
    // Static
    {
        Packet packet;

        Serialize(packet, Point{5, 6});
        Serialize(packet, Point{7, 8});
        Serialize(packet, Point{1, 2});

        std::vector<Point> points;
        Deserialize(packet, &points);

        for (const auto& point : points) {
            printf("x = %d, y = %d\n", point.x, point.y);
        }
    }

    // Polymorphic
    {
        Packet packet;

        Serialize(packet, Rectangle{1.0, 12.0});
        Serialize(packet, Circle{5.0});
        Serialize(packet, Rectangle{4.0, 5.0});

        std::vector<std::unique_ptr<Shape>> shapes;
        Deserialize(packet, &shapes);

        for (auto& shape : shapes) {
            printf("area = %lf\n", shape->area());
        }
    }

    return 0;
}