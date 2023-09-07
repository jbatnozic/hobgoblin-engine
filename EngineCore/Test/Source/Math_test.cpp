
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Math.hpp>

#include <gtest/gtest.h>

using namespace hg::math;
using namespace hg::math::angle_literals;

namespace {
constexpr double EPSILON   = 0.001;
constexpr double EPSILON_F = 0.001f;
} // namespace

TEST(HgMathTest, ShortestDistanceBetweenAnglesTest) {
    EXPECT_TRUE(( 45.0_deg).shortestDistanceTo( 50.0_deg).isEpsilonEqualTo(   5.0_deg, EPSILON));
    EXPECT_TRUE((175.0_deg).shortestDistanceTo(185.0_deg).isEpsilonEqualTo(  10.0_deg, EPSILON));
    EXPECT_TRUE((355.0_deg).shortestDistanceTo(  5.0_deg).isEpsilonEqualTo(  10.0_deg, EPSILON));
    EXPECT_TRUE(( 15.0_deg).shortestDistanceTo(345.0_deg).isEpsilonEqualTo( -30.0_deg, EPSILON));
    EXPECT_TRUE((135.0_deg).shortestDistanceTo( 45.0_deg).isEpsilonEqualTo( -90.0_deg, EPSILON));
    EXPECT_TRUE((225.0_deg).shortestDistanceTo( 45.0_deg).isEpsilonEqualTo(-180.0_deg, EPSILON));
    EXPECT_TRUE((-30.0_deg).shortestDistanceTo(-10.0_deg).isEpsilonEqualTo(  20.0_deg, EPSILON));
    EXPECT_TRUE((-30.0_deg).shortestDistanceTo( 10.0_deg).isEpsilonEqualTo(  40.0_deg, EPSILON));
    EXPECT_TRUE((700.0_deg).shortestDistanceTo( 20.0_deg).isEpsilonEqualTo(  40.0_deg, EPSILON));
}

TEST(HgMathTest, NormalizeAngleTest) {
    EXPECT_TRUE((  45.0_deg).normalize().isEpsilonEqualTo( 45.0_deg, EPSILON));
    EXPECT_TRUE(( -90.0_deg).normalize().isEpsilonEqualTo(270.0_deg, EPSILON));
    EXPECT_TRUE(( 720.0_deg).normalize().isEpsilonEqualTo(  0.0_deg, EPSILON));
    EXPECT_TRUE((-540.0_deg).normalize().isEpsilonEqualTo(180.0_deg, EPSILON));

    EXPECT_TRUE((270.0_deg).normalize(-180.0_deg, 180.0_deg).isEpsilonEqualTo(-90.0_deg, EPSILON));
    EXPECT_TRUE(( 15.0_deg).normalize( 360.0_deg, 720.0_deg).isEpsilonEqualTo(375.0_deg, EPSILON));
}

TEST(HgMathTest, IntegralCeilDivTest) {
    ASSERT_EQ(IntegralCeilDiv(0, 1), 0);

    ASSERT_EQ(IntegralCeilDiv(4, 5), 1);
    ASSERT_EQ(IntegralCeilDiv(5, 5), 1);
    ASSERT_EQ(IntegralCeilDiv(12, 5), 3);
    ASSERT_EQ(IntegralCeilDiv(15, 5), 3);

    ASSERT_EQ(IntegralCeilDiv(-6, 6), -1);
    ASSERT_EQ(IntegralCeilDiv(6, -6), -1);
    ASSERT_EQ(IntegralCeilDiv(-6, -6), 1);

    ASSERT_EQ(IntegralCeilDiv(-6, 4), -1);
}

TEST(HgMathTest, PointDirectionTest) {
    {
        SCOPED_TRACE("tests with float");

        struct MyVec {
            float x, y;
        };
        const MyVec vec0{ 0.f,   0.f};
        const MyVec vec1{20.f, -10.f};
        EXPECT_NEAR(PointDirection(vec0, vec1).asDeg(), 26.565f, EPSILON_F);

        EXPECT_NEAR(PointDirection(0.f, 0.f,  1.f,  0.f).asDeg(),   0.f, EPSILON_F);
        EXPECT_NEAR(PointDirection(0.f, 0.f,  0.f, -1.f).asDeg(),  90.f, EPSILON_F);
        EXPECT_NEAR(PointDirection(0.f, 0.f, -1.f,  0.f).asDeg(), 180.f, EPSILON_F);
        EXPECT_NEAR(PointDirection(0.f, 0.f,  0.f,  1.f).asDeg(), 270.f, EPSILON_F);
    }
    {
        SCOPED_TRACE("tests with double");

        struct MyVec {
            double x, y;
        };
        const MyVec vec0{ 0.0,   0.0};
        const MyVec vec1{20.0, -10.0};
        EXPECT_NEAR(PointDirection(vec0, vec1).asDeg(), 26.565, EPSILON);

        EXPECT_NEAR(PointDirection(0.0, 0.0,  1.0,  0.0).asDeg(),   0.0, EPSILON);
        EXPECT_NEAR(PointDirection(0.0, 0.0,  0.0, -1.0).asDeg(),  90.0, EPSILON);
        EXPECT_NEAR(PointDirection(0.0, 0.0, -1.0,  0.0).asDeg(), 180.0, EPSILON);
        EXPECT_NEAR(PointDirection(0.0, 0.0,  0.0,  1.0).asDeg(), 270.0, EPSILON);
    }
}

TEST(HgMathTest, EuclideanDistanceTest) {
    {
        SCOPED_TRACE("tests with float");

        struct MyVec {
            float x, y;
        };
        const MyVec vec0{ 0.f,   0.f};
        const MyVec vec1{20.f, -10.f};
        EXPECT_NEAR(EuclideanDist(vec0, vec1), 22.36f, EPSILON_F);
        EXPECT_NEAR(EuclideanDist(vec1, vec1),    0.f, EPSILON_F);

        EXPECT_NEAR(EuclideanDist(9.f, 4.f, 13.f, 7.f), 5.f, EPSILON_F);
        EXPECT_NEAR(EuclideanDist(9.f, 4.f,  9.f, 4.f), 0.f, EPSILON_F);
    }
    {
        SCOPED_TRACE("tests with double");

        struct MyVec {
            double x, y;
        };
        const MyVec vec0{ 0.0,   0.0};
        const MyVec vec1{20.0, -10.0};
        EXPECT_NEAR(EuclideanDist(vec0, vec1), 22.36, EPSILON);
        EXPECT_NEAR(EuclideanDist(vec1, vec1),   0.0, EPSILON);

        EXPECT_NEAR(EuclideanDist(9.0, 4.0, 13.0, 7.0), 5.0, EPSILON);
        EXPECT_NEAR(EuclideanDist(9.0, 4.0,  9.0, 4.0), 0.0, EPSILON);
    }
}

TEST(HgMathTest, RotateVectorTest) {
    {
        const auto newVec = RotateVector(5.0, -5.0, (90.0_deg).asRad());
        EXPECT_NEAR(newVec.x, -5.0, EPSILON);
        EXPECT_NEAR(newVec.y, -5.0, EPSILON);
    }
    {
        const auto newVec = RotateVector(5.0, -5.0, 180.0_deg);
        EXPECT_NEAR(newVec.x, -5.0, EPSILON);
        EXPECT_NEAR(newVec.y,  5.0, EPSILON);
    }
    {
        const auto newVec = RotateVector(7, -7, (90.0_deg).asRad());
        EXPECT_NEAR(newVec.x, -7, EPSILON);
        EXPECT_NEAR(newVec.y, -7, EPSILON);
    }
    {
        const auto newVec = RotateVector(-4.f, 0.f, 180.0_deg);
        EXPECT_NEAR(newVec.x, 4.f, EPSILON_F);
        EXPECT_NEAR(newVec.y, 0.f, EPSILON_F);
    }
    {
        const Vector2f vec{0.f, 1.f};
        const auto newVec = RotateVector(vec, (360.0_deg).asRad());
        EXPECT_NEAR(newVec.x, 0.f, EPSILON_F);
        EXPECT_NEAR(newVec.y, 1.f, EPSILON_F);
    }
    {
        const Vector2d vec{-1.0, -1.0};
        const auto newVec = RotateVector(vec, -90.0_deg);
        EXPECT_NEAR(newVec.x,  1.0, EPSILON);
        EXPECT_NEAR(newVec.y, -1.0, EPSILON);
    }
    {
        const Vector2d vec{-1.0, 1.0};
        const auto newVec = RotateVector(vec, -90.0_deg);
        EXPECT_NEAR(newVec.x, -1.0, EPSILON);
        EXPECT_NEAR(newVec.y, -1.0, EPSILON);
    }
    {
        const Vector2d vec{0.0, 9.0};
        const auto newVec = RotateVector(vec, (-180.0_deg).asRad());
        EXPECT_NEAR(newVec.x,  0.0, EPSILON);
        EXPECT_NEAR(newVec.y, -9.0, EPSILON);
    }
}
