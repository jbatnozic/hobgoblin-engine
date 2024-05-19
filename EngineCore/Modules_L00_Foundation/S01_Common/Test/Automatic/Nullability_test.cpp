// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Common.hpp>

#include <unordered_map>

#include <gtest/gtest.h>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////
// NEVERNULL TEST                                                        //
///////////////////////////////////////////////////////////////////////////

TEST(NeverNullTest, AssignmentTest) {
    // Just checking that this can compile
    int             i  = 5;
    NeverNull<int*> nn = &i;
    int*            p  = nn;
}

TEST(NeverNullTest, ComparisonTest) {
    int  vars[2] = {5, 6};
    int* p1      = &vars[0];
    int* p2      = &vars[1];

    NeverNull<int*> nn1 = &vars[0];
    NeverNull<int*> nn2 = &vars[1];

    EXPECT_FALSE(nn1 == nullptr);
    EXPECT_FALSE(nn2 == nullptr);
    EXPECT_TRUE(nn1 == p1);
    EXPECT_TRUE(p1 == nn1);
    EXPECT_TRUE(nn1 != p2);
    EXPECT_TRUE(p2 != nn1);
    EXPECT_TRUE(nn1 != nn2);
    EXPECT_FALSE(nn1 == nn2);
    EXPECT_TRUE(nn2 > nn1);
    EXPECT_TRUE(nn2 >= nn1);
    EXPECT_TRUE(nn2 > p1);
    EXPECT_TRUE(nn2 >= p1);
    EXPECT_TRUE(nn1 < p2);
    EXPECT_TRUE(nn1 <= p2);
    EXPECT_TRUE(nn1 <= p1);
}

TEST(NeverNullTest, MappingTest) {
    // Just checking that this can compile
    std::unordered_map<int, NeverNull<const void*>> map;
    map.emplace(0, this);
}

TEST(NeverNullTest, HashingTest) {
    // Just checking that this can compile
    int*            p  = reinterpret_cast<int*>(0x12345678);
    NeverNull<int*> nn = p;
    EXPECT_EQ(std::hash<decltype(p)>{}(p), std::hash<decltype(nn)>{}(nn));
}

TEST(NeverNullTest, NeverNullWithUniquePtr_1) {
    // Just checking that this can compile
    NeverNull<std::unique_ptr<int>> nn{std::make_unique<int>(123)};
}

TEST(NeverNullTest, NeverNullWithUniquePtr_2) {
    // Just checking that this can compile
    auto                            uniquePtr = std::make_unique<int>(123);
    int*                            raw       = uniquePtr.get();
    NeverNull<std::unique_ptr<int>> nn{std::move(uniquePtr)};

    EXPECT_TRUE(nn != nullptr);
    EXPECT_TRUE(nn.get().get() == raw);
}

// In Debug mode, assigning NULL to NeverNull will result in an assertion failure,
// which is difficult to test. In Release mode, it will throw an exception which
// we can easily catch.
#ifdef NDEBUG

TEST(NeverNullTest, AssignmentOfNullPointerThrowsNPE) {
    int* p = nullptr;
    EXPECT_THROW(NeverNull<int*>{p}, NullPointerException);

    int             i  = 5;
    NeverNull<int*> nn = &i;
    EXPECT_THROW({ nn = p; }, NullPointerException);
}

#endif

///////////////////////////////////////////////////////////////////////////
// AVOIDNULL TEST                                                        //
///////////////////////////////////////////////////////////////////////////

TEST(AvoidNullTest, AssignmentTest) {
    // Just checking that this can compile
    int             i  = 5;
    AvoidNull<int*> an = &i;
    int*            p  = an;
}

TEST(AvoidNullTest, ComparisonTest) {
    int  vars[2] = {5, 6};
    int* p1      = &vars[0];
    int* p2      = &vars[1];

    AvoidNull<int*> an1 = &vars[0];
    AvoidNull<int*> an2 = &vars[1];

    EXPECT_FALSE(an1 == nullptr);
    EXPECT_FALSE(an2 == nullptr);
    EXPECT_TRUE(an1 == p1);
    EXPECT_TRUE(p1 == an1);
    EXPECT_TRUE(an1 != p2);
    EXPECT_TRUE(p2 != an1);
    EXPECT_TRUE(an1 != an2);
    EXPECT_FALSE(an1 == an2);
    EXPECT_TRUE(an2 > an1);
    EXPECT_TRUE(an2 >= an1);
    EXPECT_TRUE(an2 > p1);
    EXPECT_TRUE(an2 >= p1);
    EXPECT_TRUE(an1 < p2);
    EXPECT_TRUE(an1 <= p2);
    EXPECT_TRUE(an1 <= p1);
}

TEST(AvoidNullNullTest, MappingTest) {
    // Just checking that this can compile
    std::unordered_map<int, AvoidNull<const void*>> map;
    map.emplace(0, this);
}

TEST(AvoidNullTest, HashingTest) {
    // Just checking that this can compile
    int*            p  = reinterpret_cast<int*>(0x12345678);
    AvoidNull<int*> an = p;
    EXPECT_EQ(std::hash<decltype(p)>{}(p), std::hash<decltype(an)>{}(an));
}

TEST(AvoidNullTest, AvoidNullWithUniquePtr_1) {
    // Just checking that this can compile
    AvoidNull<std::unique_ptr<int>> an{std::make_unique<int>(123)};
}

TEST(AvoidNullTest, AvoidNullWithUniquePtr_2) {
    // Just checking that this can compile
    auto uniquePtr = std::make_unique<int>(123);
    int* raw       = uniquePtr.get();

    AvoidNull<std::unique_ptr<int>> an{std::move(uniquePtr)};

    EXPECT_TRUE(an != nullptr);
    EXPECT_TRUE(an.get().get() == raw);
}

// In Debug mode, assigning NULL to NeverNull will result in an assertion failure,
// which is difficult to test. In Release mode, it will throw an exception which
// we can easily catch.
#ifdef NDEBUG

TEST(AvoidNullTest, AssignmentOfNullPointerThrowsNPE) {
    int* p = nullptr;
    EXPECT_THROW(AvoidNull<int*>{p}, NullPointerException);

    int             i  = 5;
    AvoidNull<int*> an = &i;
    EXPECT_THROW({ an = p; }, NullPointerException);
}

#endif

TEST(AvoidNullTest, MoveTest) {
    {
        int  i = 5;
        int* p = &i;

        AvoidNull<int*> an1 = p;
        AvoidNull<int*> an2 = std::move(an1);
    }
    {
        AvoidNull<std::unique_ptr<int>> an1 = std::make_unique<int>(123);
        AvoidNull<std::unique_ptr<int>> an2 = std::move(an1);
    }
}

///////////////////////////////////////////////////////////////////////////
// INTEROP TEST                                                          //
///////////////////////////////////////////////////////////////////////////

TEST(NeverNullAvoidNullInteropTest, Test1) {
    int  i = 5;
    int* p = &i;

    NeverNull<int*> nn = p;
    AvoidNull<int*> an = p;

    EXPECT_EQ(nn, an);
    EXPECT_EQ(an, nn);

    {
        NeverNull<int*> nn2 = an;
        AvoidNull<int*> an2 = nn;
    }
}

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
