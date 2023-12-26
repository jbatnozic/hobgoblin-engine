#ifndef UHOBGOBLIN_UNICODE_FMT_COMPAT_HPP
#define UHOBGOBLIN_UNICODE_FMT_COMPAT_HPP

#include <Hobgoblin/Unicode/Unicode_string.hpp>
#include <Hobgoblin/Unicode/String_conversions.hpp>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/xchar.h>

#include <string>
#include <type_traits>
#include <utility>

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

namespace detail {

//! Compile-time checker for whether a type is compatible with `fmt::format` in `char16_t` mode.
template <class taType>
using is_u16_formattable = fmt::is_formattable<taType, char16_t>;

template <class taType>
struct is_addressable {
    static const bool value =   std::is_lvalue_reference<taType>::value &&
                              ! std::is_array<std::remove_reference_t<taType>>::value &&
                              ! std::is_void<taType>::value;
};

//! Compile-time checker for whether a type has a compatible overload of `ToUnicodeString`
//! (either in the hg namespace, or in some other namespace - looked for by ADL).
template <class taType>
class supports_to_unicode_string {
    template <class taType2>
    static auto test(int)
        -> decltype( ToUnicodeString(std::declval<taType2>()), std::true_type() );

    template <class>
    static auto test(...) -> std::false_type;

public:
    static const bool value = decltype(test<taType>(0))::value;
};

using std::to_string;

//! Compile-time checker for whether a type has a compatible overload of `to_string` (either in the
//! std namespace, or in some other namespace - looked for by ADL).
template <class taType>
class supports_std_to_string {
    template <class taType2>
    static auto test(int)
        -> decltype( to_string(std::declval<taType2>()), std::true_type() );

    template <class>
    static auto test(...) -> std::false_type;

public:
    static const bool value = decltype(test<taType>(0))::value;
};

//! Compile-time checker for whether a type has a compatible stream operator into `std::ostream`.
template <class taType, class taStream>
class supports_streaming_into {
    template <class taType2, class taStream2>
    static auto test(int)
        -> decltype( std::declval<taStream2&>() << std::declval<taType2>(), std::true_type() );

    template <class, class>
    static auto test(...) -> std::false_type;

public:
    static const bool value = decltype(test<taType, taStream>(0))::value;
};

template <class taType,
          T_ENABLE_IF(detail::is_u16_formattable<taType>::value &&
                      std::is_convertible<taType, const char*>::value)>
UnicodeString ToU16FormattableImpl(taType&& aVal) {
    return UniStrConv(FROM_UTF8_STD_STRING, aVal);
}

template <class taType,
          T_ENABLE_IF(  detail::is_u16_formattable<taType>::value &&
                      ! std::is_convertible<taType, const char*>::value &&
                        detail::is_addressable<taType>::value)>
taType&& ToU16FormattableImpl(taType&& aVal) {
    return static_cast<taType&&>(aVal);
}

template <class taType,
          T_ENABLE_IF(  detail::is_u16_formattable<taType>::value &&
                      ! std::is_convertible<taType, const char*>::value &&
                      ! detail::is_addressable<taType>::value)>
taType&& ToU16FormattableImpl(taType&& aVal) {
    return static_cast<taType&&>(aVal);
}

template <class taType,
          T_ENABLE_IF(! detail::is_u16_formattable<taType>::value &&
                        detail::supports_to_unicode_string<taType>::value)>
UnicodeString ToU16FormattableImpl(taType&& aVal) {
    return ToUnicodeString(std::forward<taType>(aVal));
}

template <class taType,
          T_ENABLE_IF(! detail::is_u16_formattable<taType>::value &&
                      ! detail::supports_to_unicode_string<taType>::value &&
                        detail::supports_std_to_string<taType>::value)>
UnicodeString ToU16FormattableImpl(taType&& aVal) {
    using std::to_string;
    const std::string s = to_string(aVal);
    return UniStrConv(FROM_UTF8_STD_STRING, s);
}

template <class taType,
          T_ENABLE_IF(! detail::is_u16_formattable<taType>::value &&
                      ! detail::supports_to_unicode_string<taType>::value &&
                      ! detail::supports_std_to_string<taType>::value &&
                        detail::supports_streaming_into<taType, std::ostream>::value)>
UnicodeString ToU16FormattableImpl(taType&& aVal) {
    std::ostringstream oss;
    oss << aVal;
    return UniStrConv(FROM_UTF8_STD_STRING, oss.str());
}

template <class taType,
          T_ENABLE_IF(! detail::is_u16_formattable<taType>::value &&
                      ! detail::supports_to_unicode_string<taType>::value &&
                      ! detail::supports_std_to_string<taType>::value &&
                      ! detail::supports_streaming_into<taType, std::ostream>::value)>
void ToU16FormattableImpl(taType&& aVal) {
    static_assert(detail::is_u16_formattable<taType>::value ||
                  detail::supports_to_unicode_string<taType>::value ||
                  detail::supports_std_to_string<taType>::value ||
                  detail::supports_streaming_into<taType, std::ostream>::value, 
                  "Type not compatible with ToU16Streamable");
}

//! Turns the input argument (if possible) into something that's compatible with `hg::UFormat`:
//! 
//! - If the argument is a reference to an object that has a defined `fmt::formatter`
//!   specialization into `char16_t`, returns a reference to the original object.
//!   WARNING: This happens even if the argument is a reference to a temporary object! So,
//!            due to the lifetime extension rules, the return value of a `ToU16Formattable` call is
//!            safe to use as an argument to another function call directly, but it's NOT safe to
//!            assign it to a variable.
//! 
//! - If the argument is any kind of value or reference to an object which has no defined `fmt::formatter`
//!   specialization into `char16_t`, but DOES have a compatible overload of function 
//!   `hg::UnicodeString ToUnicodeString()` (in any namespace, found by ADL), then this function
//!   will be used to construct a `hg::UnicodeString` and return it by value.
//! 
//! - If the argument is any kind of value or reference to an object which has no defined `fmt::formatter`
//!   specialization into `char16_t` and has no compatible overload of function 
//!   `hg::UnicodeString ToUnicodeString()`, but DOES have a compatible overload of function 
//!   `std::string to_string()` (in namespace std or another namespace, found by ADL), and/or a
//!   compatible `operator<<` into `std::ostream`, then one of these* will be used to turn the
//!   argument into `std::string`, which will be converted** into `hg::UnicodeString` and returned
//!   by value.
//!   *- if both are available, `to_string` will be preferred.
//!   **- the string will be interpreted as UTF-8.
//! 
//! - If none of the above is possible, compilation will fail.
template <class taType>
HG_NODISCARD auto ToU16Formattable(taType&& aVal)
    -> decltype(detail::ToU16FormattableImpl(std::forward<taType>(aVal)))
{
    return detail::ToU16FormattableImpl(std::forward<taType>(aVal));
}

} // namespace detail

//! Similar to `fmt::format`, with the following differences:
//! - The format string needs to be UTF-16 encoded (recommended to obtain it using `HG_UNILIT` or
//!   `HG_UNIFMT` macros).
//! - The arguments will be processed accodring to the rules laid out on `detail::ToU16Formattable`.
//! - The result will be returned as `hg::UnicodeString`.
template <class taFormat, class... taArgs>
HG_NODISCARD UnicodeString UFormat(taFormat&& aFormat, taArgs&&... aArgs) {
    std::u16string u16string = fmt::format(std::forward<taFormat>(aFormat),
                                           detail::ToU16Formattable(std::forward<taArgs>(aArgs))...);
    UnicodeString result{u16string.data(), static_cast<std::int32_t>(u16string.size())};
    return result;
}

//! Converts the argument into a `hg::UnicodeString` according to the rules laid out on
//! `detail::ToU16Formattable` and returns that string.
template <class taType>
HG_NODISCARD UnicodeString UCoalesce(taType&& aValue) {
    return UFormat(HG_UNILIT("{}"), detail::ToU16Formattable(std::forward<taType>(aValue)));
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
