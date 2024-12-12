// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_FILE_IO_HPP
#define UHOBGOBLIN_UTIL_FILE_IO_HPP

#include <filesystem>
#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

//! Returns a bitwise copy of the contents of a file (no checks or conversions
//! will be performed so the returned string might not be printable).
std::string SlurpFileBytes(const std::filesystem::path& aPath);

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_FILE_IO_HPP
