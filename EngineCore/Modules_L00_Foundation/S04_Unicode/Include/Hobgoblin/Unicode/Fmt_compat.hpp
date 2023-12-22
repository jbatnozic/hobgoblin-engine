#ifndef UHOBGOBLIN_UNICODE_FMT_COMPAT_HPP
#define UHOBGOBLIN_UNICODE_FMT_COMPAT_HPP

#include <Hobgoblin/Unicode/Unicode_string.hpp>
#include <Hobgoblin/Unicode/String_conversions.hpp>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/xchar.h>

#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

namespace fmt {

// We use the u16string_view formatter as the base as u16string_view is already
// made to handle UTF-16 string made on char16_t, same as ICU's UnicodeString.
#define BASE_FORMATTER ::fmt::formatter<std::u16string_view, char16_t>

template <>
struct formatter<::jbatnozic::hobgoblin::UnicodeString, char16_t> : BASE_FORMATTER {
    template <class taContext>
    auto format(const ::jbatnozic::hobgoblin::UnicodeString& aUniStr, taContext& aContext) const {
        const std::u16string_view sview{aUniStr.getBuffer(), 
                                        static_cast<std::size_t>(aUniStr.length())};
        return BASE_FORMATTER::format(sview, aContext);
    }
};

#undef BASE_FORMATTER

} // namespace fmt

HOBGOBLIN_NAMESPACE_BEGIN

//template <class taType>
//class is_u16_formattable {
//    template <class taType2>
//    static auto test(int)
//        -> decltype( fmt::format(u"{}", std::declval<taType2>()), std::true_type() );
//
//    template <class>
//    static auto test(...) -> std::false_type;
//
//public:
//    static const bool value = decltype(test<taType>(0))::value;
//};

template <class taType>
using is_u16_formattable = fmt::is_formattable<taType, char16_t>;

template <class taType>
class is_tostringable {
    template <class taType2>
    static auto test(int)
        -> decltype( std::to_string(std::declval<taType2>()), std::true_type() );

    template <class>
    static auto test(...) -> std::false_type;

public:
    static const bool value = decltype(test<taType>(0))::value;
};

template <class taStream, class taType>
class is_streamable {
    template <class taStream2, class taType2>
    static auto test(int)
        -> decltype( std::declval<taStream2&>() << std::declval<taType2>(), std::true_type() );

    template <class, class>
    static auto test(...) -> std::false_type;

public:
    static const bool value = decltype(test<taStream, taType>(0))::value;
};

template <class taType,
          T_ENABLE_IF(is_u16_formattable<taType>::value)>
constexpr taType&& Stringify(taType&& aValue) {
    return static_cast<taType&&>(aValue);
}

template <class taType,
          T_ENABLE_IF(!is_u16_formattable<taType>::value &&
                      is_tostringable<taType>::value)>
UnicodeString Stringify(taType&& aValue) {
    std::string s = std::to_string(aValue);
    return UniStrConv(FROM_UTF8_STD_STRING, s);
}

template <class taType,
          T_ENABLE_IF(!is_u16_formattable<taType>::value &&
                      !is_tostringable<taType>::value &&
                      is_streamable<std::ostream, taType>::value)>
UnicodeString Stringify(taType&& aValue) {
    std::ostringstream oss;
    oss << aValue;
    return UniStrConv(FROM_UTF8_STD_STRING, oss.str());
}

//! Same as `fmt::format` but it converts the result into a `hg::UnicodeString`.
template <class taFormat, class... taArgs>
[[nodiscard]] UnicodeString UFormat(taFormat&& aFormat, taArgs&&... aArgs) {
    std::u16string u16string = fmt::format(std::forward<taFormat>(aFormat),
                                           Stringify(std::forward<taArgs>(aArgs))...);
    UnicodeString result{u16string.data(), static_cast<std::int32_t>(u16string.size())};
    return result;
}

//! This macro expands into a UTF-16 encoded string literal which can be used as the format
//! string of `hg::UFormat()`. A compile-time check will be performed to verify that the format
//! string is well-formed.
//! 
//! \param _c_string_literal_ a plain C string literal without any prefixes.
//! 
//! \note make sure that the source files containing this macro invocation are saved with
//!       proper encoding that supports unicode.
#define HG_UNIFMT(_c_string_literal_) FMT_STRING(HG_UNILIT(_c_string_literal_))

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UNICODE_FMT_COMPAT_HPP
