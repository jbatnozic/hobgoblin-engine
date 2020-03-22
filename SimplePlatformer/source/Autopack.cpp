
#include <Hobgoblin/Utility/Autopack.hpp>
#include <Hobgoblin/Utility/Packet.hpp>

struct TEST {
    TEST(int i = 0, float f = 0.0f)
        : _i{i}, _f{f}
    {
    }

    int getI() const {
        return _i;
    }

    float getF() const {
        return _f;
    }

private:
    int _i;
    float _f;
    HG_ENABLE_AUTOPACK(TEST, _i, _f);
};

int main() {
    TEST t1{2, 3.14f};

    hg::util::Packet packet;
    packet << t1;

    TEST t2 = packet.extractOrThrow<TEST>();
    std::cout << t2.getI() << ' ' << t2.getF() << '\n';

    return 0;
}