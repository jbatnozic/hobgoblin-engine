
#include <GridWorld/Renderer.hpp>

#include <gtest/gtest.h>

#include <array>

#if 0
namespace gridworld {

TEST(GridWorldSpatialInfoDrawingOrderTest, FourSeparateRectangles) {
    std::array<SpatialInfo, 4> infos;

    infos[0].setTopLeft(World{6.5f, 1.f});
    infos[0].setSize({1.5f, 1.5f});

    infos[1].setTopLeft({2.f, 3.f});
    infos[1].setSize({4.f, 2.f});

    infos[2].setTopLeft({4.5f, 7.f});
    infos[2].setSize({2.f, 3.f});

    infos[3].setTopLeft({1.f, 6.f});
    infos[3].setSize({3.f, 3.f});

    for (unsigned i = 0; i < 4; i += 1) {
        for (unsigned t = i + 1; t < 4; t += 1) {
            EXPECT_EQ(infos[i].checkDimetricDrawingOrder(infos[t]), SpatialInfo::DRAW_THIS_FIRST);
            EXPECT_EQ(infos[t].checkDimetricDrawingOrder(infos[i]), SpatialInfo::DRAW_OTHER_FIRST);
        }
    }
}

TEST(GridWorldSpatialInfoDrawingOrderTest, TouchingRectanglesCycle) {
    std::array<gridworld::SpatialInfo, 4> infos;

    infos[0].setTopLeft({2.0f, 0.f});
    infos[0].setSize({6.f, 2.f});

    infos[1].setTopLeft({6.f, 2.f});
    infos[1].setSize({2.f, 6.f});

    infos[2].setTopLeft({0.f, 0.f});
    infos[2].setSize({2.f, 6.f});

    infos[3].setTopLeft({0.f, 6.f});
    infos[3].setSize({6.f, 2.f});

    for (unsigned i = 0; i < 4; i += 1) {
        for (unsigned t = i + 1; t < 4; t += 1) {
            EXPECT_EQ(infos[i].checkDimetricDrawingOrder(infos[t]), SpatialInfo::DRAW_THIS_FIRST);
            EXPECT_EQ(infos[t].checkDimetricDrawingOrder(infos[i]), SpatialInfo::DRAW_OTHER_FIRST);
        }
    }
}

// TODO(doesn't matter - test)

TEST(GridWorldSpatialInfoDrawingOrderTest, CircleOrbitingAnotherCircle) {
    // SpatialInfos are technically rectangles, not circles, but we pretend they are
    // circles with R=2, and we set the spatial info params according to the bounding
    // boxes of those circles.
    // Pivot orbits the axis with a distance of 4 between their centres.

    const float R = 2.f; // circle radius
    const float centreX = 8.f;
    const float centreY = 8.f;

    gridworld::SpatialInfo axis;
    axis.setCentre({centreX, centreX});
    axis.setSizeMaintainingCentre({2.f * R, 2.f * R});

    gridworld::SpatialInfo pivot;
    pivot.setSizeMaintainingCentre({2.f * R, 2.f * R});

    for (int i = 0; i < 360; i += 1) {
        const auto angle = hg::math::AngleF::fromDegrees(static_cast<float>(i - 45));
        const float x = centreX + angle.cos() * 4.f;
        const float y = centreY - angle.sin() * 4.f;
        pivot.setCentre({x, y});

        const auto order = pivot.checkDimetricDrawingOrder(axis);
        if (i == 0 || i == 180) {
            // It's indeterminate what happens in this case (and also doesn't matter)
            continue;
        } else if (i <= 180) {
            EXPECT_EQ(order, SpatialInfo::DRAW_THIS_FIRST) << "angle = " << i;
        } else {
            EXPECT_EQ(order, SpatialInfo::DRAW_OTHER_FIRST) << "angle = " << i;
        }
    }
}

} // namespace gridworld
#endif
