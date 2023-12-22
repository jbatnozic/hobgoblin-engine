
#include <Hobgoblin/Unicode.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <iostream>

#include <fmt/ostream.h>

namespace hg = jbatnozic::hobgoblin;

std::ostream& operator<<(std::ostream& os, char16_t c) {
    return os << (std::int32_t)c;
}

TEST(HGUnicodeTest, TestSerbianLatin) {
    static constexpr int32_t  EXPECTED_LENGTH = 14;
    static constexpr char16_t EXPECTED_CONTENTS[] = {
        0x0160, 0x0107, 0x010d, 0x0020, 0x0061, 0x0073, 0x0064,
        0x0066, 0x0020, 0x0111, 0x0161, 0x017e, 0x002e, 0x007e
    };

    const auto str = HG_UNISTR("Šćč asdf đšž.~");
    ASSERT_EQ(str.length(), EXPECTED_LENGTH);
    const char16_t* buffer = str.getBuffer();
    
    for (int32_t i = 0; i < EXPECTED_LENGTH; i += 1) {
        EXPECT_EQ(buffer[i], EXPECTED_CONTENTS[i]);
        EXPECT_EQ(str[i],    EXPECTED_CONTENTS[i]);
    }

    // icu::UnicodeString is NOT null-terminated!
    // EXPECT_EQ(buffer[EXPECTED_LENGTH], 0);
}

TEST(HGUnicodeTest, TestSerbianCyrillic) {
    static constexpr int32_t  EXPECTED_LENGTH = 16;
    static constexpr char16_t EXPECTED_CONTENTS[] = {
        0x0069, 0x0074, 0x0020, 0x0438, 0x0437, 0x0020, 0x0432, 0x0445,
        0x0430, 0x0442, 0x0020, 0x0438, 0x0442, 0x0020, 0x0069, 0x0073
    };

    const auto str = HG_UNISTR("it из вхат ит is");
    ASSERT_EQ(str.length(), EXPECTED_LENGTH);
    const char16_t* buffer = str.getBuffer();

    for (int32_t i = 0; i < EXPECTED_LENGTH; i += 1) {
        EXPECT_EQ(buffer[i], EXPECTED_CONTENTS[i]);
        EXPECT_EQ(str[i],    EXPECTED_CONTENTS[i]);
    }
}

TEST(HGUnicodeTest, TestWeirdAlphabets) {
    static constexpr int32_t  EXPECTED_LENGTH = 5;
    static constexpr char16_t EXPECTED_CONTENTS[] = {
        0x0061, 0x002d, 0x4e2d, 0x002d, 0x042f
    };

    const auto str = HG_UNISTR("a-中-Я");
    ASSERT_EQ(str.length(), EXPECTED_LENGTH);
    const char16_t* buffer = str.getBuffer();

    for (int32_t i = 0; i < EXPECTED_LENGTH; i += 1) {
        EXPECT_EQ(buffer[i], EXPECTED_CONTENTS[i]);
        EXPECT_EQ(str[i],    EXPECTED_CONTENTS[i]);
    }
}

namespace {
std::string SlurpFileContents(std::filesystem::path aPath) {
    std::ifstream fileStream{aPath, std::ios::in | std::ios::binary};
    std::string fileContents{std::istreambuf_iterator<char>{fileStream},
                             std::istreambuf_iterator<char>()};
    return fileContents;
}
} // namespace

TEST(HGUnicodeTest, TestConversionToPath) {
    const auto basePath     = hg::UnicodeString{HG_TEST_ASSET_DIR};
    const auto relativePath = HG_UNISTR("čćdšzžj/гроњћчвељ.txt");
    const auto path         = hg::UniStrConv(hg::TO_STD_PATH, basePath + '/' + relativePath);

    ASSERT_TRUE(std::filesystem::exists(path));

    const auto fileContents = SlurpFileContents(path);

    EXPECT_EQ(fileContents, "TEST");
}

TEST(HGUnicodeTest, TestCharsetDetection_UTF8) {
    struct TestCase {
        hg::UnicodeString relativeFilePath;
        hg::Charset       expectedCharset;
        std::int32_t      expectedConfidence;
    };

    const TestCase TEST_CASES[] = {
        {
            HG_UNILIT("ChuckNorris/ISO-8859-1.txt"),
            hg::Charset::ISO_8859_1,
            23
        },
        {
            HG_UNILIT("ChuckNorris/UTF-8.txt"),
            hg::Charset::ISO_8859_1, // With no BOM and no non-ASCII characters, there is no way to
            23                       // distinguish between UTF-8 and ISO-8859-1.
        },
        {
            HG_UNILIT("ChuckNorris/UTF-8_wUnicodeChars.txt"),
            hg::Charset::UTF_8,
            80
        },
        {
            HG_UNILIT("ChuckNorris/UTF-8_BOM.txt"),
            hg::Charset::UTF_8,
            100
        },
        {
            HG_UNILIT("ChuckNorris/UTF-8_BOM_wUnicodeChars.txt"),
            hg::Charset::UTF_8,
            100
        },
        {
            HG_UNILIT("ChuckNorris/UTF-16_BE_BOM.txt"),
            hg::Charset::UTF_16_BE,
            100
        },
        {
            HG_UNILIT("ChuckNorris/UTF-16_BE_BOM_wUnicodeChars.txt"),
            hg::Charset::UTF_16_BE,
            100
        },
        {
            HG_UNILIT("ChuckNorris/UTF-16_LE_BOM.txt"),
            hg::Charset::UTF_16_LE,
            100
        },
        {
            HG_UNILIT("ChuckNorris/UTF-16_LE_BOM_wUnicodeChars.txt"),
            hg::Charset::UTF_16_LE,
            100
        }
    };

    for (const auto& testCase : TEST_CASES) {
        const auto basePath     = hg::UnicodeString{HG_TEST_ASSET_DIR};
        const auto path         = hg::UniStrConv(hg::TO_STD_PATH, basePath + '/' + testCase.relativeFilePath);

        ASSERT_TRUE(std::filesystem::exists(path));

        const auto fileContents = SlurpFileContents(path);

        const auto detectionResult = hg::CharsetDetector(fileContents.c_str(), 
                                                         static_cast<std::int32_t>(fileContents.size())).detect();

        EXPECT_EQ(detectionResult.charset, testCase.expectedCharset)
            << "for file: " << hg::UniStrConv(hg::TO_ASCII_STD_STRING, testCase.relativeFilePath);

        EXPECT_EQ(detectionResult.confidence, testCase.expectedConfidence)
            << "for file: " << hg::UniStrConv(hg::TO_ASCII_STD_STRING, testCase.relativeFilePath);
    }
}

// TODO(Test newline processing)

namespace {

struct CustomFormattedType {
    char c = 0;
    int  i = 0;
};

std::ostream& operator<<(std::ostream& aOStream, const CustomFormattedType& aCFT) {
    return (aOStream << aCFT.c << ' ' << aCFT.i);
}

} // namespace

//template <>
//struct fmt::formatter<CustomFormattedType, char16_t> : fmt::basic_ostream_formatter<char16_t> {};

TEST(HGUnicodeTest, FmtTest) {
    {
        const auto expectedString  = HG_UNISTR("The answer is 42.");
        const auto formattedString = hg::UFormat(HG_UNIFMT("The answer is {}."), 42);

        EXPECT_EQ(expectedString, formattedString);
    }
    {
        const auto expectedString  = HG_UNISTR("The answer is 42.");
        const auto formattedString = hg::UFormat(HG_UNIFMT("The {1} is {0}."), 42, HG_UNILIT("answer"));

        EXPECT_EQ(expectedString, formattedString);
    }
    {
        const auto expectedString  = HG_UNISTR("The string 'šđžčć' contains unicode characters such as ž.");
        const auto formattedString = hg::UFormat(HG_UNIFMT("The string '{}' contains unicode characters such as ž."),
                                                 HG_UNILIT("šđžčć"));

        EXPECT_EQ(expectedString, formattedString);
    }
    {
        const auto expectedString  = HG_UNISTR("The string 'šđžčć' contains unicode characters such as ž.");
        const auto formattedString = hg::UFormat(HG_UNIFMT("The string '{}' contains unicode characters such as ž."),
                                                 HG_UNISTR("šđžčć"));

        EXPECT_EQ(expectedString, formattedString);
    }
    {
        const auto expectedString  = HG_UNISTR("The CPU is i 7.");
        const auto formattedString = hg::UFormat(HG_UNIFMT("The CPU is {}."), CustomFormattedType{'i', 7});

        EXPECT_EQ(expectedString, formattedString);
    }
    {
        using TYPE = CustomFormattedType;

        const auto e1 = hg::is_u16_formattable<TYPE>::value;

        const auto e2 = hg::is_tostringable<TYPE>::value;

        const auto e3 = hg::is_streamable<std::ostream, TYPE>::value;
    }
}
