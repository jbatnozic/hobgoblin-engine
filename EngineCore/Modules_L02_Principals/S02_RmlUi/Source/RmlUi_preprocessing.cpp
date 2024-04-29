// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#include <Hobgoblin/RmlUi/RmlUi_preprocessing.hpp>

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Sprite_loader.hpp>
#include <Hobgoblin/Graphics/Tga_export.hpp>

#include <cctype>
#include <fstream>
#include <functional>
#include <optional>
#include <regex>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>
HOBGOBLIN_NAMESPACE_BEGIN
namespace rml {

#define FAIL(_msg_) HG_THROW_TRACED(PreprocessingError, 0, _msg_)

namespace {
struct LineInfo {
    std::string wholeLine;
    std::string leadingWhitespace;
    std::string commandName;
    std::vector<std::string> commandArgs;
    bool isCommand = false;
};

class Parser {
public:
    LineInfo& parseLine(std::string aLine) {
        std::smatch smatch;
        if (std::regex_match(aLine, smatch, _regex)) {
            _lineInfo.wholeLine = std::move(aLine);
            _lineInfo.leadingWhitespace = smatch[1];
            _lineInfo.commandName = smatch[2];
            _lineInfo.commandArgs = _parseArgs(smatch[3]);
            _lineInfo.isCommand = true;
        } else {
            _lineInfo.wholeLine = std::move(aLine);
            _lineInfo.leadingWhitespace.clear();
            _lineInfo.commandName.clear();
            _lineInfo.commandArgs.clear();
            _lineInfo.isCommand = false;
        }
        return _lineInfo;
    }

private:
    constexpr static auto BASE_PATTERN = R"_(^(\s*)%%fp:([_a-zA-Z0-9:]+)(.*)\s*$)_";
    const std::regex _regex{BASE_PATTERN};
    LineInfo _lineInfo;

    static std::vector<std::string> _parseArgs(const std::string& aArgs) {
        enum State {
            Search,     // Searching for non-whitespace characters
            Normal,     // Appending to current token
            UnderQuotes // Appending to current token which is under quotes
        } state = Search;

        std::vector<std::string> result;
        std::vector<char> stack;

        for (const char c : aArgs) {
            switch (state) {
                case Search:
                    if (std::isspace(c)) {
                        // do nothing
                    } else if (c == '"') {
                        state = UnderQuotes;
                    } else {
                        stack.push_back(c);
                        state = Normal;
                    }
                    break;

                case Normal:
                    if (std::isspace(c)) {
                        stack.push_back('\0');
                        result.emplace_back(stack.data());
                        stack.clear();
                        state = Search;
                    }
                    else if (c == '"') {
                        stack.push_back('\0');
                        result.emplace_back(stack.data());
                        stack.clear();
                        state = UnderQuotes;
                    }
                    else {
                        stack.push_back(c);
                    }
                    break;

                case UnderQuotes:
                    if (std::isspace(c)) {
                        stack.push_back(c);
                    }
                    else if (c == '"') {
                        stack.push_back('\0');
                        result.emplace_back(stack.data());
                        stack.clear();
                        state = Search;
                    }
                    else {
                        stack.push_back(c);
                    }
                    break;

                default:
                    break;
            }
        }

        if (!stack.empty()) {
            stack.push_back('\0');
            result.emplace_back(stack.data());
        }

        return result;
    }
};

void ForEachLineInFileDo(const std::string& aFilePath, 
                         const std::function<void(std::string aLine)>& aAction) {
    std::fstream file{aFilePath, std::ios::in};
    std::string line;
    while (std::getline(file, line)) {
        aAction(std::move(line));
    }
}

struct SrcResult {
    AvoidNull<std::unique_ptr<gr::SpriteLoader::TextureBuilder>> texBuilder;
    std::string path;
};

// %%fp:src: -f <path> -w <width> -h <height>
SrcResult ProcessSrcCommand(LineInfo& aLineInfo,
                            gr::SpriteLoader& aSpriteLoader) {
    if (aLineInfo.commandArgs.size() != 6) {
        FAIL("Wrong format for 'src' definition - use \"-f <path> -w <width> -h <height>\"");
    }

    PZInteger width, height;
    std::string path;
    bool widthFound = false, heightFound = false, pathFound = false;

    for (std::size_t i = 0; i < aLineInfo.commandArgs.size() - 1; i += 1) {
        if (aLineInfo.commandArgs[i] == "-f") {
            path = std::move(aLineInfo.commandArgs[i + 1]);
            pathFound = true;
            i += 1;
        } else if (aLineInfo.commandArgs[i] == "-w") {
            width = std::stoi(aLineInfo.commandArgs[i + 1]);
            widthFound = true;
            i += 1;
        } else if (aLineInfo.commandArgs[i] == "-h") {
            height = std::stoi(aLineInfo.commandArgs[i + 1]);
            heightFound = true;
            i += 1;
        } else {
            FAIL(""); // TODO
        }
    }

    if (!pathFound || !widthFound || !heightFound) {
        FAIL("Wrong format for 'src' definition - use \"-f <path> -w <width> -h <height>\"");
    }

    return {aSpriteLoader.startTexture(width, height), std::move(path)};
}

struct SpriteResult {
    std::string name;
};

// %%fp:sprite: -n <name> -f <path>
SpriteResult ProcessSpriteCommand(LineInfo& aLineInfo,
                                  const std::string& aContainingFolder,
                                  gr::SpriteLoader::TextureBuilder& aTexBuilder) {
    if (aLineInfo.commandArgs.size() != 4) {
        FAIL("Wrong format for 'sprite' definition - use \"-n <name> -f <path>\"");
    }

    std::string name, path;
    bool nameFound = false, pathFound = false;

    for (std::size_t i = 0; i < aLineInfo.commandArgs.size() - 1; i += 1) {
        if (aLineInfo.commandArgs[i] == "-n") {
            name = std::move(aLineInfo.commandArgs[i + 1]);
            nameFound = true;
            i += 1;
        }
        else if (aLineInfo.commandArgs[i] == "-f") {
            path = std::move(aLineInfo.commandArgs[i + 1]);
            pathFound = true;
            i += 1;
        }
        else {
            FAIL(""); // TODO
        }
    }

    if (!nameFound || !pathFound) {
        FAIL("Wrong format for 'sprite' definition - use \"-n <name> -f <path>\"");
    }

    aTexBuilder.addSubsprite(name, 0, aContainingFolder + path);

    return {std::move(name)};
}

void ProcessSpritesheetEndCommand(SrcResult& aSrcResult,
                                  const std::string& aContainingFolder) {
    const auto& texture = aSrcResult.texBuilder->finalize(gr::TexturePackingHeuristic::BestAreaFit); // TODO hardcoded heuristic
    gr::ExportTextureToTgaFile(texture, aContainingFolder + aSrcResult.path);
}

std::string GetContainingFolder(std::string aPath) {
    {
        const auto pos = aPath.find_last_of('/');
        if (pos != std::string::npos) {
            while (aPath.length() > pos + 1) {
                aPath.pop_back();
            }
            return aPath;
        }
    }
    {
        const auto pos = aPath.find_last_of('\\');
        if (pos != std::string::npos) {
            while (aPath.length() > pos + 1) {
                aPath.pop_back();
            }
            return aPath;
        }
    }
    return "";
}

} // namespace

void PreprocessRcssFile(const std::string& aFilePath) {
    if (aFilePath.length() < 9 || aFilePath.substr(aFilePath.length() - 8, 8) != ".rcss.fp") {
        // ERROR: file path must end in .rcss.fp
        FAIL("extension"); // TODO
    }

    const auto containingFolder = GetContainingFolder(aFilePath);
    Parser parser;
    gr::SpriteLoader loader;
    std::optional<SrcResult> srcResult;

    std::vector<std::string> outputLines;

    bool spritesheetBegan = false;
    bool sourceProvided   = false;

    const auto lineAction = [&](std::string aLine) {
        auto& lineInfo = parser.parseLine(std::move(aLine));
        if (!lineInfo.isCommand) {
            outputLines.emplace_back(std::move(lineInfo.wholeLine));
        } else {
            // %% fp:spritesheet_begin
            if (lineInfo.commandName == "spritesheet_begin") {
                if (spritesheetBegan) {
                    FAIL("Unexpected 'spritesheet_begin' encountered!");
                }
                spritesheetBegan = true;
                outputLines.push_back(lineInfo.leadingWhitespace + "/* spritesheet began here */");
            }
            // %%fp:spritesheet_end
            else if (lineInfo.commandName == "spritesheet_end") {
                if (!spritesheetBegan) {
                    FAIL("Unexpected 'spritesheet_end' encountered!");
                }
                ProcessSpritesheetEndCommand(*srcResult, containingFolder);
                spritesheetBegan = false;
                outputLines.push_back(lineInfo.leadingWhitespace + "/* spritesheet ended here */");
            }
            // %%fp:src: -f <path> -w <width> -h <height>
            else if (lineInfo.commandName == "src:") {
                if (sourceProvided) {
                    FAIL("Only one 'src' per spritesheet is supported!");
                }
                if (!spritesheetBegan) {
                    FAIL("A 'spritesheet_begin' must come before any 'src' definitions!");
                }
                sourceProvided = true;
                srcResult.emplace(ProcessSrcCommand(lineInfo, loader));
                outputLines.push_back(lineInfo.leadingWhitespace + "src: " + srcResult->path + ";");
            }
            // %%fp:sprite: -n window-tl  -f ./sprites/window-tl.png
            else if (lineInfo.commandName == "sprite:") {
                if (!sourceProvided) {
                    FAIL("A 'src' definition must come before any 'sprite' definitions!");
                }
                if (!spritesheetBegan) {
                    FAIL("A 'spritesheet_begin' must come before any 'sprite' definitions!");
                }
                const auto result = ProcessSpriteCommand(lineInfo, containingFolder, *(srcResult->texBuilder));
                outputLines.push_back(lineInfo.leadingWhitespace + "sprite: %%{" + result.name + "}");
            }
            // ???
            else {
                FAIL("Unrecognised command encountered!");
            }
        }
    };

    ForEachLineInFileDo(aFilePath, lineAction);

    // Use gathered/generated information to output a new .rcss file:
    {
        const auto outFileName = aFilePath.substr(0, aFilePath.length() - 3); // erase .fp extension
        std::fstream outFile{outFileName, std::ios::out | std::ios::trunc};

        constexpr static auto SPRITE_ID_PATTERN = R"_(^(.*)sprite: %%\{(.+)\}$)_";
        std::regex spriteIdRegex{SPRITE_ID_PATTERN};

        for (const auto& line : outputLines) {
            std::smatch smatch;
            // If line contains an incomplete sprite declaration, fill it in here:
            if (std::regex_match(line, smatch, spriteIdRegex)) {
                const auto sprite = loader.getBlueprint(smatch[2]).spr();
                const auto coords = sprite.getTextureRect();

                // format: x y width height
                outFile
                    << smatch[1] // leading whitespace
                    << smatch[2] // sprite name
                    << ": "
                    << coords.x << "px "
                    << coords.y << "px "
                    << coords.w << "px "
                    << coords.h << "px;"
                    << '\n';
            }
            // Otherwise, just output the original line unchanged:
            else {
                outFile << line << '\n';
            }
        }
    }
}

} // namespace rml
HOBGOBLIN_NAMESPACE_END
#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
