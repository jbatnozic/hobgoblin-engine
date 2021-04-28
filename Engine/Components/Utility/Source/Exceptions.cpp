
#include <Hobgoblin/Utility/Exceptions.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

namespace {

void GenerateStackTrace(std::vector<std::string>& target) {
    // TODO
}

} // namespace

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
        os << string;
    }
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>