#ifndef UHOBGOBLIN_UNICODE_UNICODE_STRING_HPP
#define UHOBGOBLIN_UNICODE_UNICODE_STRING_HPP

#include <unicode/unistr.h>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

//! A unicode-aware string that uses UTF-16 encoding.
using UnicodeString = ::icu::UnicodeString;

//! This macro expands into a UTF-16 encoded string literal of type `const char16_t*`.
//! String literals created in this way are interoperable with `hg::UnicodeString` and can also
//! be concatenated by the preprocessor if placed side-by-side.
//! 
//! \param _c_string_literal_ a plain C string literal without any prefixes.
//! 
//! \note make sure that the source files containing this macro invocation are saved with
//!       proper encoding that supports unicode.
#define HG_UNILIT(_c_string_literal_) \
    u##_c_string_literal_

//! This macro expands into an object of type `hg::UnicodeString`.
//! 
//! \param _c_string_literal_ a plain C string literal without any prefixes. The returned object
//!                           will contain this string.
//! 
//! \note make sure that the source files containing this macro invocation are saved with
//!       proper encoding that supports unicode.
#define HG_UNISTR(_c_string_literal_) \
    (::jbatnozic::hobgoblin::UnicodeString{HG_UNILIT(_c_string_literal_)})

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UNICODE_UNICODE_STRING_HPP
