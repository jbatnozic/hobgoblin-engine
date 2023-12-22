
#include <Hobgoblin/Unicode/String_conversions.hpp>

#include <unicode/schriter.h>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

std::string UniStrConv(TO_UTF8_STD_STRING_Tag, const UnicodeString& aUnicodeString) {
    std::string result;
    aUnicodeString.toUTF8String(result);
    return result;
}

std::string UniStrConv(TO_ASCII_STD_STRING_Tag,
                       const UnicodeString& aUnicodeString,
                       char aUnsupportedCharFiller) {
    std::string result;
    icu::StringCharacterIterator iter{aUnicodeString};
    while (iter.hasNext()) {
        const UChar32 c = iter.next32PostInc();
        if (c >= 0 && c <= 127) {
            result.push_back(static_cast<char>(c));
        }
        else {
            result.push_back(aUnsupportedCharFiller);
        }
    }
    return result;
}

std::u8string UniStrConv(TO_STD_U8STRING_Tag, const UnicodeString& aUnicodeString) {
    class U8ByteSink : public icu::ByteSink {
    public:
        std::u8string u8string;

        void Append(const char* aBytes, int32_t aN) override {
            u8string.append(
                reinterpret_cast<const char8_t*>(aBytes),
                static_cast<std::size_t>(aN)
            );
        }
    };

    U8ByteSink sink;
    aUnicodeString.toUTF8(sink);
    std::u8string result{std::move(sink.u8string)};
    return result;
}

std::filesystem::path UniStrConv(TO_STD_PATH_Tag, const UnicodeString& aUnicodeString) {
    return {UniStrConv(TO_STD_U8STRING, aUnicodeString)};
}

auto UniStrConv(FROM_UTF8_STD_STRING_Tag, const std::string& aStdString) -> UnicodeString {
    return UnicodeString{aStdString.data(), static_cast<std::int32_t>(aStdString.size()), "UTF-8"};
}

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
