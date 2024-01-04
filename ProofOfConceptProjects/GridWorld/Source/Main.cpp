
#include <GridWorld/Renderer.hpp>

#include <Hobgoblin/Math.hpp>

#include <array>
#include <iostream>

namespace hg = jbatnozic::hobgoblin;

#define OLD

#if 0
int main2() {
#ifdef OLD // Hobgoblin coordinate system (Y goes down)
    // CASE 1:
    std::cout << "CASE 1:\n";
    {
        std::array<gridworld::SpatialInfo, 4> infos;

        infos[0].position = {6.5f, 1.f, 0.f};
        infos[0].dimensions = {1.5f, 1.5f};

        infos[1].position = {2.f, 3.f, 0.f};
        infos[1].dimensions = {4.f, 2.f};

        infos[2].position = {4.5f, 7.f, 0.f};
        infos[2].dimensions = {2.f, 3.f};

        infos[3].position = {1.f, 6.f, 0.f};
        infos[3].dimensions = {3.f, 3.f};

        for (unsigned i = 0; i < 4; i += 1) {
            for (unsigned t = i + 1; t < 4; t += 1) {
                std::cout << infos[i].checkDrawingOrder(infos[t]) << '\n';
            }
        }
    }
    // CASE 2:
    std::cout << "CASE 2:\n";
    {
        std::array<gridworld::SpatialInfo, 4> infos;

        infos[0].position = {2.0f, 0.f, 0.f};
        infos[0].dimensions = {6.f, 2.f};

        infos[1].position = {6.f, 2.f, 0.f};
        infos[1].dimensions = {2.f, 6.f};

        infos[2].position = {0.f, 0.f, 0.f};
        infos[2].dimensions = {2.f, 6.f};

        infos[3].position = {0.f, 6.f, 0.f};
        infos[3].dimensions = {6.f, 2.f};

        for (unsigned i = 0; i < 4; i += 1) {
            for (unsigned t = i + 1; t < 4; t += 1) {
                std::cout << infos[i].checkDrawingOrder(infos[t]) << '\n';
            }
        }
    }
    // CASE 3:
    std::cout << "CASE 3:\n";
    {
        gridworld::SpatialInfo axis;
        axis.position = {8.f, 8.f, 0.f};
        axis.dimensions = {2.f, 2.f};

        gridworld::SpatialInfo pivot;
        pivot.dimensions = {2.f, 2.f};

        for (int i = 0; i < 360; i += 1) {
            const auto angle = hg::math::AngleF::fromDegrees(static_cast<float>(i - 45));
            const float x = axis.position.x + angle.cos() * 4.f;
            const float y = axis.position.y - angle.sin() * 4.f;
            pivot.position = {x, y, 0.f};
            std::cout << "angle = " << (i - 0) << "; result = " << pivot.checkDrawingOrder(axis) << '\n';
        }
    }
#endif
#ifndef OLD // mathematical coordinate system (Y goes up)
    // CASE 1:
    std::cout << "CASE 1:\n";
    {
        std::array<gridworld::SpatialInfo, 4> infos;

        infos[0].position = {6.5f, 6.5f, 0.f};
        infos[0].dimensions = {1.5f, 1.5f};

        infos[1].position = {2.f, 6.f, 0.f};
        infos[1].dimensions = {4.f, 2.f};

        infos[2].position = {4.5f, 1.f, 0.f};
        infos[2].dimensions = {2.f, 3.f};

        infos[3].position = {1.f, 2.f, 0.f};
        infos[3].dimensions = {3.f, 3.f};

        for (unsigned i = 0; i < 4; i += 1) {
            for (unsigned t = i + 1; t < 4; t += 1) {
                std::cout << infos[i].checkDrawingOrder(infos[t]) << '\n';
            }
        }
    }
    // CASE 2:
    std::cout << "CASE 2:\n";
    {
        std::array<gridworld::SpatialInfo, 4> infos;

        infos[0].position = {3.f, 8.f, 0.f};
        infos[0].dimensions = {6.f, 2.f};

        infos[1].position = {7.f, 2.f, 0.f};
        infos[1].dimensions = {2.f, 6.f};

        infos[2].position = {1.f, 4.f, 0.f};
        infos[2].dimensions = {2.f, 6.f};
        ;
        infos[3].position = {1.f, 2.f, 0.f};
        infos[3].dimensions = {6.f, 2.f};

        for (unsigned i = 0; i < 4; i += 1) {
            for (unsigned t = i + 1; t < 4; t += 1) {
                std::cout << infos[i].checkDrawingOrder(infos[t]) << '\n';
            }
        }
    }
    // CASE 3:
    std::cout << "CASE 3:\n";
    {
        gridworld::SpatialInfo axis;
        axis.position = {8.f, 8.f, 0.f};
        axis.dimensions = {2.f, 2.f};

        gridworld::SpatialInfo pivot;
        pivot.dimensions = {2.f, 2.f};

        for (int i = 0; i < 360; i += 1) {
            const auto angle = hg::math::AngleF::fromDegrees(static_cast<float>(i - 45));
            const float x = axis.position.x + angle.cos() * 4.f;
            const float y = axis.position.y + angle.sin() * 4.f;
            pivot.position = {x, y, 0.f};
            std::cout << "angle = " << (i - 45) << "; result = " << pivot.checkDrawingOrder(axis) << '\n';
        }
    }
#endif
    return 0;
}
#endif
