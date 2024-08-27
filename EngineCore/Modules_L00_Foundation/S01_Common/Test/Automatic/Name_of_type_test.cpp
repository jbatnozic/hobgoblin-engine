
// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Common/Name_of_type.hpp>

#include <gtest/gtest.h>

#include <Hobgoblin/Private/Pmacro_define.hpp>

namespace ns123 {

using Handle = short;

struct MyStruct {};

template <class T>
struct MyTemplateStruct {};

} // namespace ns123

HOBGOBLIN_NAMESPACE_BEGIN

TEST(NameOfTypeTest, Test1) {
    EXPECT_EQ(GetNameOfType<int>(), std::string{"int"});
    EXPECT_EQ(GetNameOfType<unsigned int>(), std::string{"unsigned int"});
    EXPECT_EQ(GetNameOfType<ns123::Handle>(), std::string{"short"});
    EXPECT_EQ(GetNameOfType<ns123::MyStruct>(), std::string{"ns123::MyStruct"});
    EXPECT_EQ(GetNameOfType<ns123::MyTemplateStruct<float>>(),
              std::string{"ns123::MyTemplateStruct<float>"});
}

HOBGOBLIN_NAMESPACE_END
