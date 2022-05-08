#ifndef UHOBGOBLIN_LOGGING_SEVERITY_HPP
#define UHOBGOBLIN_LOGGING_SEVERITY_HPP

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace log {

//! TODO(description)
enum class Severity {
    All     = 0,
    Debug   = 1,
    Info    = 2,
    Warning = 3,
    Error   = 4,
    Fatal   = 5
};

//! TODO(description)
HG_DYNAPI void HGCALL SetMinimalLogSeverity(Severity aSeverity);

//! TODO(description)
HG_DYNAPI Severity HGCALL GetMinimalLogSeverity();

} // namespace log
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_LOGGING_SEVERITY_HPP