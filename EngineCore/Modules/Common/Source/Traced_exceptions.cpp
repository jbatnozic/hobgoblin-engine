
#include <Hobgoblin/Common/Traced_exceptions.hpp>

#include <sstream>

#include <Hobgoblin/Private/Pmacro_define.hpp>

#if defined(_WIN32) || defined(WIN32)
#include <Windows.h>
#include <DbgHelp.h>

static void GenerateStackTrace(std::vector<std::string>& target) {
#define NUM_FRAMES 50

    unsigned int i;
    void* stack[NUM_FRAMES];
    unsigned short frames;
    SYMBOL_INFO* symbol;
    HANDLE process;

    process = GetCurrentProcess();

    SymInitialize(process, NULL, TRUE);

    frames = CaptureStackBackTrace(0, NUM_FRAMES, stack, NULL);

    symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
    if (symbol == NULL) {
        return;
    }

    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    for (i = 0; i < frames; i++)
    {
        SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);

        std::stringstream ss;

        ss << (frames - i - 1) << ": " << symbol->Name << " - " << std::hex << symbol->Address;

        target.push_back(ss.str());
    }

    free(symbol);

#undef NUM_FRAMES
}
#else
static void GenerateStackTrace(std::vector<std::string>& target) {
    // No impl
}
#endif

HOBGOBLIN_NAMESPACE_BEGIN

TracedException::TracedException()
    : _message{}
{
    GenerateStackTrace(_stackTrace);
}

TracedException::TracedException(const char* message)
    : _message{message}
{
    GenerateStackTrace(_stackTrace);
}

TracedException::TracedException(const std::string& message)
    : _message{message} 
{
    GenerateStackTrace(_stackTrace);
}

const char* TracedException::what() const noexcept {
    return _message.c_str();
}

const std::string& TracedException::whatString() const noexcept {
    return _message;
}

const std::vector<std::string>& TracedException::getStackTrace() const {
    return _stackTrace;
}

void TracedException::printStackTrace(std::ostream& os) const {
    for (auto& string : _stackTrace) {
        os << string << '\n';
    }
}

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>