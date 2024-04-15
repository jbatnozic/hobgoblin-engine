// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Config.hpp>

#include <gtest/gtest.h>
#include <string>

using hg::HGConfig;

TEST(HGConfigTest, EmptyHGConfigCanBeConstructed) {
    HGConfig hgconf;
}

TEST(HGConfigTest, ConstructionFromSimpleString) {
    const std::string configstr = R"_(
        [[Section1]]
        Variable [one-of: "A", "B", "C"] := "A"
    )_";
    HGConfig hgconf{configstr};
}

TEST(HGConfigTest, ConstructionFromComplexString) {
    const std::string configstr = R"_(
        // Comment //

        [[Section1]]
        Variable1 [unchecked:] := "a6s87c9c7s6b97s8f7637498fbh8309"
        Variable2 [one-of: "A", "B", "C"] := "A"
        Variable3 [integer:] := "123"
        Variable4 [real:][min-value: "0.0"][max-value: "1000.0"] := "123.456"

        [[Section2]]
        Variable1 [regex: ".*"] := "abcdefghijkl 0123456789 +-"
    )_";
    HGConfig hgconf{configstr};
}

TEST(HGConfigTest, ConstructionFromComplexStringWithRandomSpacing) {
    const std::string configstr = R"_(
            // Comment //

        [[Section1]]
        Variable1[ one-of   : "A","B", "C"] :=    "A"   
        Variable2 [integer:  ] := "123"
        Variable3 [real : ][    min-value: "0.0"][max-value: "1000.0"] :=    "123.456"

           [[ Section2  ]] 
        Variable1 [    regex: ".*"] := "abcdefghijkl 0123456789 +-"

        // Another comment  


    )_";
    HGConfig hgconf{configstr};
}

TEST(HGConfigTest, ConstructionFromSimpleStringWithTabs) {
    const std::string configstr = "[[\tSection1\t]]\n"
                                  "Variable1[ one-of\t: \"A\",\"B\", \"C\"] :=    \"A\"\t";
    HGConfig hgconf{configstr};
}

TEST(HGConfigTest, ConstructionFromComplexStringWithLinesBrokenByBackslashes) {
    const std::string configstr = R"_(
        // Comment //

        [[Section1]]
        Variable1 \
            [one-of: "A", \
                     "B", \
                     "C"] := "A"

        Variable2 [integer:] \
        := \
        "123"

        Variable3 [real:][min-value: "0.0"] \
        [max-value: "1000.0"] := "123.456"
    )_";
    HGConfig hgconf{configstr};
}

TEST(HGConfigTest, CanNotBreakCommentWithBackslash) {
    const std::string configstr = R"_(
        // Comm \
        ent //

        [[Section1]]
        Variable1 \
            [one-of: "A", \
                     "B", \
                     "C"] := "A"
    )_";
    EXPECT_THROW(HGConfig{configstr}, hg::HGConfigParseError);
}

TEST(HGConfigTest, CanNotBreakSectionWithBackslash) {
    const std::string configstr = R"_(
        [[Section1 \
        ]]
        Variable1 \
            [one-of: "A", \
                     "B", \
                     "C"] := "A"
    )_";
    EXPECT_THROW(HGConfig{configstr}, hg::HGConfigParseError);
}

TEST(HGConfigTest, CanNotAddTwoSectionsWithSameName) {
    const std::string configstr = R"_(
        [[Section1]]
        [[Section1]]
    )_";
    EXPECT_THROW(HGConfig{configstr}, hg::HGConfigSemanticError);
}

TEST(HGConfigTest, CanNotAddTwoEntriesWithSameNameInSameSection) {
    const std::string configstr = R"_(
        [[Section1]]
        Variable1 [one-of: "A", "B", "C"] := "A"
        Variable1 [one-of: "A", "B", "C"] := "A"
    )_";
    EXPECT_THROW(HGConfig{configstr}, hg::HGConfigSemanticError);
}

TEST(HGConfigTest, ConstructionFailsDueToMissingOrWrongTokens) {
    // Missing section name 
    EXPECT_THROW(HGConfig{R"_(
                 [[]]
                 Variable1 [one-of: "A", "B", "C"] := "A"
                 )_"}, 
                 hg::HGConfigSemanticError); // [[]] is recognised as a malformed entry declaration thus
                                             // "no section defined" semantic error is thrown
    // Brackets in section header can't be separated
    EXPECT_THROW(HGConfig{R"_(
                 [[Section1] ]
                 Variable1 [one-of: "A", "B", "C"] := "A"
                 )_"}, 
                 hg::HGConfigSemanticError); // [[]] is recognised as a malformed entry declaration thus
                                             // "no section defined" semantic error is thrown
    // Missing colon after label
    EXPECT_THROW(HGConfig{R"_(
                 [[Section1]]
                 Variable1 [one-of "A", "B", "C"] := "A"
                 )_"}, 
                 hg::HGConfigParseError);
    // Missing "
    EXPECT_THROW(HGConfig{R"_(
                 [[Section1]]
                 Variable1 [one-of: "A", "B", "C"] := A"
                 )_"}, 
                 hg::HGConfigParseError);
    // Entry name can't start with number
    EXPECT_THROW(HGConfig{R"_(
                 [[Section1]]
                 0Variable1 [one-of: "A", "B", "C"] := "A"
                 )_"}, 
                 hg::HGConfigParseError);
    // Entry name can't start with dash
    EXPECT_THROW(HGConfig{R"_(
                 [[Section1]]
                 -Variable1 [one-of: "A", "B", "C"] := "A"
                 )_"}, 
                 hg::HGConfigParseError);
    // Entry name missing
    EXPECT_THROW(HGConfig{R"_(
                 [[Section1]]
                 [one-of: "A", "B", "C"] := "A"
                 )_"}, 
                 hg::HGConfigParseError);
    // Main descriptor
    EXPECT_THROW(HGConfig{R"_(
                 [[Section1]]
                 Variable1 := "A"
                 )_"}, 
                 hg::HGConfigParseError);
    // Colon in assignment missing
    EXPECT_THROW(HGConfig{R"_(
                 [[Section1]]
                 Variable1 [one-of: "A", "B", "C"] = "A"
                 )_"}, 
                 hg::HGConfigParseError);
    // Comma in parameter list missing
    EXPECT_THROW(HGConfig{R"_(
                 [[Section1]]
                 Variable1 [one-of: "A" "B", "C"] := "A"
                 )_"}, 
                 hg::HGConfigParseError);
    // Invalid one-of value
    EXPECT_THROW(HGConfig{R"_(
                 [[Section1]]
                 Variable1 [one-of: "A", "B", "C"] := "D"
                 )_"}, 
                 hg::HGConfigSemanticError);
    // Value doesn't match regex
    EXPECT_THROW(HGConfig{R"_(
                 [[Section1]]
                 Variable1 [regex: "^[a-zA-Z]$"] := "0123"
                 )_"}, 
                 hg::HGConfigSemanticError);
    // Value cannot be parsed as integer
    EXPECT_THROW(HGConfig{R"_(
                 [[Section1]]
                 Variable1 [integer:] := "asd+-+//"
                 )_"}, 
                 hg::HGConfigParseError);
    // Value cannot be parsed as real
    EXPECT_THROW(HGConfig{R"_(
                 [[Section1]]
                 Variable1 [integer:] := "asd+-+//"
                 )_"}, 
                 hg::HGConfigParseError);
}

TEST(HGConfigTest, ReadRawValues) {
    const std::string configstr = R"_(
        [[Section1]]
        Variable1 [one-of: "A", "B", "C"] := "A"
        Variable2 [integer:] := "123"

        [[Section2]]
        Variable1 [regex: ".*"] := "abcdefghijkl 0123456789 +-"
    )_";
    HGConfig hgconf{configstr};

    ASSERT_TRUE(hgconf.sectionExists("Section1"));
    ASSERT_TRUE(hgconf.entryExists("Section1", "Variable1"));
    ASSERT_TRUE(hgconf.entryExists("Section1", "Variable2"));
    ASSERT_EQ(hgconf.getRawValue("Section1", "Variable1"), "A");
    ASSERT_EQ(hgconf.getRawValue("Section1", "Variable2"), "123");

    ASSERT_TRUE(hgconf.sectionExists("Section2"));
    ASSERT_TRUE(hgconf.entryExists("Section2", "Variable1"));
    ASSERT_EQ(hgconf.getRawValue("Section2", "Variable1"), "abcdefghijkl 0123456789 +-");

    ASSERT_FALSE(hgconf.sectionExists("Section3"));
    ASSERT_FALSE(hgconf.entryExists("Section2", "Variable3"));
}

TEST(HGConfigTest, ReadTypedValues) {
    const std::string configstr = R"_(
        [[Section1]]
        Variable1 [unchecked:] := "a6s87c9c7s6b97s8f7637498fbh8309"
        Variable2 [one-of: "Asdf", "Bijk", "Cwer"] := "Asdf"
        Variable3 [regex: ".*"] := "__some_value__"
        Variable4 [integer:] := "1234"
        Variable5 [real:] := "12.34"

        [[Section2]]
        Variable1 [integer:][min-value: "0"][max-value: "10"] := "5"
    )_";
    HGConfig hgconf{configstr};

    EXPECT_EQ(hgconf.getStringValue("Section1", "Variable1"), "a6s87c9c7s6b97s8f7637498fbh8309");
    EXPECT_EQ(hgconf.getStringValue("Section1", "Variable2"), "Asdf");
    EXPECT_EQ(hgconf.getStringValue("Section1", "Variable3"), "__some_value__");
    EXPECT_EQ(hgconf.getIntegerValue("Section1", "Variable4"), 1234);
    EXPECT_EQ(hgconf.getRealValue("Section1", "Variable5"), std::stod("12.34"));

    const auto& entry = hgconf.getEntry("Section2", "Variable1");
    EXPECT_EQ(entry.kind, HGConfig::EntryKind::Integer);
    EXPECT_EQ(entry.value, "5");
    EXPECT_EQ(entry.mainDescriptor.label, "integer");
    EXPECT_TRUE(entry.mainDescriptor.args.empty());
    EXPECT_EQ(entry.extraDescriptors.size(), 2);
    EXPECT_EQ(entry.extraDescriptors[0].label, "min-value");
    ASSERT_EQ(entry.extraDescriptors[0].args.size(), 1);
    EXPECT_EQ(entry.extraDescriptors[0].args[0], "0");
    EXPECT_EQ(entry.extraDescriptors[1].label, "max-value");
    ASSERT_EQ(entry.extraDescriptors[1].args.size(), 1);
    EXPECT_EQ(entry.extraDescriptors[1].args[0], "10");
}

TEST(HGConfigTest, DumpStateWithoutChange) {
    const std::string configstr = R"_(
        // Comment

        [[Section1]]
        Variable1 \
            [one-of: "A", \
                     "B", \
                     "C"] := "A"
        [[ _section_2_ ]]
        var-2 [regex: ".*"]  :=  "aS DF "

        //  Comment   //
    )_";
    HGConfig hgconf{configstr};

    ASSERT_EQ(configstr, hgconf.dumpState());
}

TEST(HGConfigTest, DumpStateAfterChange) {
    const std::string configstr = R"_(
        // Comment

        [[Section1]]
        Variable1 \
            [one-of: "A", \
                     "B", \
                     "C"] := "A"
        [[ _section_2_ ]]
        var-2 [regex: ".*"]  :=  "aS DF "

        //  Comment   //
    )_";
    HGConfig hgconf{configstr};
    // TODO (no modifying functions yet...)
}

TEST(HGConfigTest, GetUnsignedInteger) {
    const std::string configstr = R"_(
        [[Section1]]
        Variable1 [integer:] := "18446744073709551615"
    )_";
    HGConfig hgconf{configstr};
    EXPECT_EQ(hgconf.getIntegerValue<std::uint64_t>("Section1", "Variable1"), 18446744073709551615ULL);
}

TEST(HGConfigTest, GetIntegerWithCustomRadix) {
    const std::string configstr = R"_(
        [[Section1]]
        Variable1 [integer:]   := "0xd3ecf5726d81ccb3"
        Variable2 [unchecked:] := "  d3ecf5726d81ccb3"
    )_";
    HGConfig hgconf{configstr};
    EXPECT_EQ(hgconf.getIntegerValue<std::uint64_t>("Section1", "Variable1", 16), 0xd3ecf5726d81ccb3);
    EXPECT_EQ(hgconf.getIntegerValue<std::uint64_t>("Section1", "Variable2", 16), 0xd3ecf5726d81ccb3);
}

// clang-format on
