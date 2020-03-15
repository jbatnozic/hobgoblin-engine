
#include <gtest/gtest.h>

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Utility/Any_ptr.hpp>

using namespace hg::util;

TEST(AnyPtrTest, IntPtr) {
    AnyPtr p;
    int dummy;

    p.reset(&dummy);

    ASSERT_EQ(p.get<int>(), &dummy);
}

TEST(AnyPtrTest, VoidPtr) {
    AnyPtr p;
    void* pVoid;
    pVoid = &pVoid;

    p.reset(pVoid);

    ASSERT_EQ(p.get<void>(), pVoid);
}

TEST(AnyPtrTest, TypeMismatchFailsAssert) {
    AnyPtr p;
    int dummy;

    p.reset(&dummy);

    EXPECT_DEATH_IF_SUPPORTED(p.get<double>(), ".*");
}

TEST(AnyPtrTest, TypeMismatchThrows) {
    AnyPtr p;
    int dummy;

    p.reset(&dummy);

    EXPECT_THROW(p.getOrThrow<double>(), TracedLogicError);
}

TEST(AnyPtrTest, ConstPtr) {
    AnyPtr p;
    const int constDummy = 5;

    p.reset(&constDummy);

    ASSERT_EQ(p.get<const int>(), &constDummy);
}

TEST(AnyPtrTest, ConstPtrTypeMismatchThrows) {
    AnyPtr p;
    const int constDummy = 5;

    p.reset(&constDummy);

    ASSERT_THROW(p.getOrThrow<int>(), TracedLogicError);
}