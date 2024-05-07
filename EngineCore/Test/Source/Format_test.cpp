// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/Format.hpp>

#include <gtest/gtest.h>

TEST(FormatTest, CheckFmtIsLinked) {
    EXPECT_EQ(fmt::format("{} is {}!", "This", "Hobgoblin"),
              "This is Hobgoblin!");
}

// clang-format on
