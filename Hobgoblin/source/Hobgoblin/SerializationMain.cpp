
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Utility/Serialization.hpp>

#include <iostream>

using namespace hg::util;

struct Point : public Serializable {
    int x;
    int y;

    Point(int xx, int yy) : x{xx}, y{yy} {}

    void serialize(Packet& packet) const override { // TODO Rename to rawSerialize or something...
        packet << x << y;
    }

    std::string getSerializationTag() const { // TODO Generate automatically
        return "Point";
    }
};

void Create_Point(Packet& packet, AnyPtr context) {
    std::vector<Point>& points = *context.getOrThrow<std::vector<Point>>();
    int x = packet.extractOrThrow<int>();
    int y = packet.extractOrThrow<int>();
    points.emplace_back(x, y);
}

detail::StaticSerializableObjectMappingInitializer ssomi{"Point", Create_Point};

int main() {
    Packet packet;

    Serialize(packet, Point{5, 6});
    Serialize(packet, Point{7, 8});
    Serialize(packet, Point{1, 2});

    std::vector<Point> points;
    Deserialize(packet, &points);

    for (const auto& point : points) {
        printf("x = %d, y = %d\n", point.x, point.y);
    }

    return 0;
}