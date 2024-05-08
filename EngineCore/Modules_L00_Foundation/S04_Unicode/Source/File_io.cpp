// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Unicode/File_io.hpp>

#include <Hobgoblin/HGExcept.hpp>

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

namespace {
std::string SlurpFileContents(std::filesystem::path aPath) {
    std::ifstream fileStream{aPath, std::ios::in | std::ios::binary};
    std::string fileContents{std::istreambuf_iterator<char>{fileStream},
        std::istreambuf_iterator<char>()};
    return fileContents;
}
} // namespace

UnicodeString LoadWholeFile(const std::filesystem::path& aPath,
                            std::optional<Charset> aCharset) {
    const auto contents = SlurpFileContents(aPath);

    Charset charset;
    if (aCharset.has_value()) {
        charset = *aCharset;
    } else {
        const auto detectionResult = CharsetDetector(contents.c_str(), 
                                                     static_cast<std::int32_t>(contents.size())).detect();
        charset = detectionResult.charset;
    }

    const char* codepage = nullptr;
    switch (charset) {
    case Charset::ISO_8859_1:
        codepage = "ISO-8859-1";
        break;

    case Charset::UTF_8:
        codepage = "UTF-8";
        break;

    case Charset::UTF_16_LE:
        codepage = "UTF-16LE";
        break;

    case Charset::UTF_16_BE:
        codepage = "UTF-16BE";
        break;

    default:
        HG_THROW_TRACED(TracedRuntimeError, 0, "Could not detect charset of file at '{}'.", aPath.string());
    }

    return UnicodeString{contents.data(),
                         static_cast<std::int32_t>(contents.size()),
                         codepage};
}

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
