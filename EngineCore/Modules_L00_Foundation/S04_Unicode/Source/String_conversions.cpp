
#include <Hobgoblin/Unicode/String_conversions.hpp>

#include <SFML/System/String.hpp>

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

auto UniStrConv(FROM_ASCII_STD_STRING_Tag, const std::string& aStdString) -> UnicodeString {
    return UniStrConv(FROM_UTF8_STD_STRING, aStdString);
}

auto UniStrConv(FROM_STD_U8STRING_Tag, const std::u8string& aStdU8String) -> UnicodeString {
    return UnicodeString{
        reinterpret_cast<const char*>(aStdU8String.data()),
        static_cast<std::int32_t>(aStdU8String.size()),
        "UTF-8"
    };
}

namespace detail {
void LoadUStringFromSfString(UnicodeString& aUniStr, const void* aSfString) {
    const auto& sfString = *static_cast<const sf::String*>(aSfString);

    if (sfString.getSize() == 0) {
        aUniStr.remove();
        return;
    }

    aUniStr = UnicodeString::fromUTF32(
        reinterpret_cast<const UChar32*>(sfString.getData()),
        static_cast<std::int32_t>(sfString.getSize())
    );
}

void StoreUStringInSfString(const UnicodeString& aUniStr, void* aSfString) {
    auto& sfString = *static_cast<sf::String*>(aSfString);
    sfString.clear();

    sf::String temp{sf::Uint32{'_'}};
    icu::StringCharacterIterator iter{aUniStr};
    while (iter.hasNext()) {
        const auto codePoint = iter.next32PostInc();
        if (codePoint == decltype(iter)::DONE) {
            break;
        }
        temp[0] = codePoint;
        sfString.insert(sfString.getSize(), temp);
    }
}
} // namespace detail

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
