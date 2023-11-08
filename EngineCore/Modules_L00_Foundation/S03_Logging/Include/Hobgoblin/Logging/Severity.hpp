#ifndef UHOBGOBLIN_LOGGING_SEVERITY_HPP
#define UHOBGOBLIN_LOGGING_SEVERITY_HPP

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace log {

//! Severity of log messages.
enum class Severity {
    All     = 0,

    //! This log level should be used for information that may
    //! be needed for diagnosing issues and troubleshooting.
    Debug   = 1,

    //! The standard log level indicating that something (regular
    //! and expected) happened.
    Info    = 2,

    //! This log level signifies that something unexpected
    //! happened but the application can continue to work.
    Warning = 3,

    //! The log level that should be used when the application
    //! hits an issue preventing one or more functionalities from
    //! properly functioning.
    Error   = 4,

    //! The log level that tells that the application encountered
    //! an event or entered a state in which one of the crucial
    //! business functionality is no longer working.
    Fatal   = 5,
};

//! Sets the minimal logging severity (log messages with lower severity
//! will be completely ignored). You can set it to Severity::All to never
//! ignore any messages.
//! By default it is Severity::Warning.
HG_DYNAPI void HGCALL SetMinimalLogSeverity(Severity aSeverity);

//! Returns the current minimal logging severity set by SetMinimalLogSeverity().
HG_DYNAPI Severity HGCALL GetMinimalLogSeverity();

} // namespace log
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_LOGGING_SEVERITY_HPP
