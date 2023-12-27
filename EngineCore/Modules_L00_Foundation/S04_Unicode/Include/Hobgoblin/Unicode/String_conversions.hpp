#ifndef UHOBGOBLIN_UNICODE_STRING_CONVERSIONS_HPP
#define UHOBGOBLIN_UNICODE_STRING_CONVERSIONS_HPP

#include <Hobgoblin/Common/Tag_types.hpp>
#include <Hobgoblin/Unicode/Unicode_string.hpp>

#include <filesystem>
#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

HG_DECLARE_TAG_TYPE(TO_UTF8_STD_STRING);
HG_DECLARE_TAG_TYPE(TO_ASCII_STD_STRING);
HG_DECLARE_TAG_TYPE(TO_STD_U8STRING);
HG_DECLARE_TAG_TYPE(TO_STD_PATH);
HG_DECLARE_TAG_TYPE(FROM_UTF8_STD_STRING);
HG_DECLARE_TAG_TYPE(FROM_ASCII_STD_STRING);
HG_DECLARE_TAG_TYPE(FROM_STD_U8STRING);

//! Transcodes a `UnicodeString` into UTF-8, then stores it in a `std::string`
//! and returns it.
//! 
//! \note This is provided for compatibility with older APIs and it's recommended
//!       to use `std::u8string` where possible.
auto UniStrConv(TO_UTF8_STD_STRING_Tag, const UnicodeString& aUnicodeString) -> std::string;

//! Transcodes a `UnicodeString` into ASCII, then stores it in a `std::string`
//! and returns it.
//! Code points not representable in ASCII will be replaced with
//! `aUnsupportedCharFiller`.
auto UniStrConv(TO_ASCII_STD_STRING_Tag, 
                const UnicodeString& aUnicodeString,
                char aUnsupportedCharFiller = '?') -> std::string;

//! Transcodes a `UnicodeString` into UTF-8, then stores it in a `std::u8string`
//! and returns it.
auto UniStrConv(TO_STD_U8STRING_Tag, const UnicodeString& aUnicodeString) -> std::u8string;

//! Returns a `std::filesystem::path` constructed from the given string.
auto UniStrConv(TO_STD_PATH_Tag, const UnicodeString& aUnicodeString) -> std::filesystem::path;

//! TODO(add description)
auto UniStrConv(FROM_UTF8_STD_STRING_Tag, const std::string& aStdString) -> UnicodeString;

//! TODO(add description)
auto UniStrConv(FROM_ASCII_STD_STRING_Tag, const std::string& aStdString) -> UnicodeString;

//! TODO(add description)
auto UniStrConv(FROM_STD_U8STRING_Tag, const std::u8string& aStdU8String) -> UnicodeString;

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UNICODE_STRING_CONVERSIONS_HPP
