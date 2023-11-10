#include <Hobgoblin/HGExcept/Traced_exception.hpp>

#include <sstream>
#include <utility>

#if defined(_WIN32) || defined(WIN32)
#include <Windows.h>
#include <DbgHelp.h>

static void CaptureStackTrace(std::vector<std::string>& target) {
#define NUM_FRAMES 50

    unsigned int i;
    void* stack[NUM_FRAMES];
    unsigned short frames;
    SYMBOL_INFO* symbol;
    HANDLE process = GetCurrentProcess();

    SymInitialize(process, NULL, TRUE);

    frames = CaptureStackBackTrace(0, NUM_FRAMES, stack, NULL);

    symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
    if (symbol == NULL) {
        return;
    }

    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    int counter = 0;
    for (i = 0; i < frames; i++) {
        if (i < 2) {
            continue; // to skip TracedException constructor and CaptureStackTrace
        }

        SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);

        std::ostringstream oss;

        oss << counter << ": " << symbol->Name << " - " << std::hex << symbol->Address;

        target.push_back(oss.str());

        counter++;
    }

    free(symbol);

#undef NUM_FRAMES
}
#else
static void CaptureStackTrace(std::vector<std::string>& target) {
    // No impl
}
#endif

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

TracedException::TracedException(
    std::string aType,
    std::string aFunc,
    std::string aFile,
    int aLineNumber,
    int aErrorCode,
    std::string aErrorMessage,
    std::string aComment
) 
    : _type{std::move(aType)}
    , _func{std::move(aFunc)}
    , _file{std::move(aFile)}
    , _lineNumber{aLineNumber}
    , _errorCode{aErrorCode}
    , _errorMessage{std::move(aErrorMessage)}
    , _comment{aComment.empty() ? "<none>" : std::move(aComment)}
    , _description{
        fmt::format(
            "Traced exception of type '{}' thrown from '{}' in '{}:{}' with error code '{}', message: '{}', and comment: '{}'.",
            _type,
            _func,
            _file,
            _lineNumber,
            _errorCode,
            _errorMessage,
            _comment
        )}
{
    CaptureStackTrace(_stackTrace);
}

const std::string& TracedException::getType() const noexcept {
    return _type;
}

const std::string& TracedException::getFunction() const noexcept {
    return _func;
}

const std::string& TracedException::getFile() const noexcept {
    return _file;
}

int TracedException::getLineNumber() const noexcept {
    return _lineNumber;
}

int TracedException::getErrorCode() const noexcept {
    return _errorCode;
}

const std::string& TracedException::getErrorMessage() const noexcept {
    return _errorMessage;
}

const std::vector<std::string>& TracedException::getStackTrace() const noexcept {
    return _stackTrace;
}

const std::string& TracedException::getDescription() const noexcept {
    return _description;
}

std::string TracedException::getFormattedDescription() const noexcept {
    std::ostringstream oss;
    oss << "Traced exception of type '" << _type << "' thrown\n"
        << "        FROM FUNC: " << _func << '\n'
        << "        FROM FILE: " << _file << ':' << _lineNumber << '\n'
        << "        WITH CODE: " << _errorCode << '\n'
        << "          MESSAGE: " << _errorMessage << '\n'
        << "          COMMENT: " << _comment;
    return oss.str();
}

std::string TracedException::getFullFormattedDescription() const noexcept {
    std::ostringstream oss;
    oss << "Traced exception of type '" << _type << "' thrown\n"
        << "        FROM FUNC: " << _func << '\n'
        << "        FROM FILE: " << _file << ':' << _lineNumber << '\n'
        << "        WITH CODE: " << _errorCode << '\n'
        << "          MESSAGE: " << _errorMessage << '\n'
        << "          COMMENT: " << _comment << "\n\n"
        << "    STACK TRACE:\n";
    bool haveAtLeastOne = false;
    for (const auto& str : _stackTrace) {
        oss << "        " << str << '\n';
        haveAtLeastOne = true;
    }
    auto result = oss.str();
    if (haveAtLeastOne) {
        result.pop_back();
    }
    return result;
}

const char* TracedException::what() const noexcept {
    return _description.c_str();
}

HOBGOBLIN_NAMESPACE_END
