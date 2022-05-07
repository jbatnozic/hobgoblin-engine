
#include <Hobgoblin/Format.hpp>

#include <gtest/gtest.h>

TEST(FormatTest, CheckFmtIsLinked) {
    EXPECT_EQ(fmt::format("{} is {}!", "This", "Hobgoblin"),
              "This is Hobgoblin!");
}
