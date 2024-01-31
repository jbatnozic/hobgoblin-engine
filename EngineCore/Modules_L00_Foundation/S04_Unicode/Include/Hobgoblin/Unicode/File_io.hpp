#ifndef UHOBGOBLIN_UNICODE_FILE_IO_HPP
#define UHOBGOBLIN_UNICODE_FILE_IO_HPP

#include <Hobgoblin/Unicode/Charset_detection.hpp>
#include <Hobgoblin/Unicode/Unicode_string.hpp>

#include <filesystem>
#include <optional>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

//! Loads the whole contents of a textual file into a unicode string.
//! \param aPath Path to the file.
//! \param aCharset Encoding charset of the file. If none, the
//!                 function will try to detect it automatically.
UnicodeString LoadWholeFile(const std::filesystem::path& aPath,
                            std::optional<Charset> aCharset = {});

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UNICODE_FILE_IO_HPP
