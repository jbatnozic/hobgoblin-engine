#ifndef UHOBGOBLIN_UTIL_EXCEPTIONS_HPP
#define UHOBGOBLIN_UTIL_EXCEPTIONS_HPP

#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

class TracedException : public std::exception {
public:
    explicit TracedException();
    explicit TracedException(const char* message);
    explicit TracedException(const std::string& message);

    const char* what() const noexcept override;
    const std::string& whatString() const noexcept;

    const std::vector<std::string>& getStackTrace() const;
    void printStackTrace(std::ostream& os) const;

private:
    std::string _message;
    std::vector<std::string> _stackTrace;
};

class TracedLogicError : public TracedException {
public:
    using TracedException::TracedException;
};

class TracedRuntimeError : public TracedException {
public:
    using TracedException::TracedException;
};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_EXCEPTIONS_HPP
