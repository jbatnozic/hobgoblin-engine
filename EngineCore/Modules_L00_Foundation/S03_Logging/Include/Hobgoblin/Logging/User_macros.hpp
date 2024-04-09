#ifndef UHOBGOBLIN_LOGGING_USER_MACROS_HPP
#define UHOBGOBLIN_LOGGING_USER_MACROS_HPP

#include <Hobgoblin/Logging/Impl.hpp>
#include <Hobgoblin/Logging/Severity.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>
HOBGOBLIN_NAMESPACE_BEGIN
namespace log {

//! Use this macro to log a DEBUG message if the minimal logging severity
//! is at least DEBUG (see `SetMinimalLogSeverity`).
//! \param _log_id_ Identifier of the component from which the message is
//!                 being logged. This parameter must be of type `const char*`
//!                 or be convertible to it.
//! \param _format_ Plain C string literal giving the format of the logged
//!                 message (see `fmt` library and Hobgoblin.Format).
//! \param ...      [optional] variables to be formatted into the logged message
//!                 as described by the _format_ parameter.
#define HG_LOG_DEBUG(_log_id_, _format_, ...) \
    do{ UHOBGOBLIN_LOG_IMPL(::jbatnozic::hobgoblin::log::Severity::Debug, \
                            _log_id_, \
                            _format_, \
                            __VA_ARGS__ \
                            ); } while (false)

//! Use this macro to log an INFO message if the minimal logging severity
//! is at least INFO (see `SetMinimalLogSeverity`).
//! \param _log_id_ Identifier of the component from which the message is
//!                 being logged. This parameter must be of type `const char*`
//!                 or be convertible to it.
//! \param _format_ Plain C string literal giving the format of the logged
//!                 message (see `fmt` library and Hobgoblin.Format).
//! \param ...      [optional] variables to be formatted into the logged message
//!                 as described by the _format_ parameter.
#define HG_LOG_INFO(_log_id_, _format_, ...) \
    do{ UHOBGOBLIN_LOG_IMPL(::jbatnozic::hobgoblin::log::Severity::Info, \
                            _log_id_, \
                            _format_, \
                            __VA_ARGS__ \
                            ); } while (false)

//! Use this macro to log a WARN message if the minimal logging severity
//! is at least WARNING (see `SetMinimalLogSeverity`).
//! \param _log_id_ Identifier of the component from which the message is
//!                 being logged. This parameter must be of type `const char*`
//!                 or be convertible to it.
//! \param _format_ Plain C string literal giving the format of the logged
//!                 message (see `fmt` library and Hobgoblin.Format).
//! \param ...      [optional] variables to be formatted into the logged message
//!                 as described by the _format_ parameter.
#define HG_LOG_WARN(_log_id_, _format_, ...) \
    do{ UHOBGOBLIN_LOG_IMPL(::jbatnozic::hobgoblin::log::Severity::Warning, \
                            _log_id_, \
                            _format_, \
                            __VA_ARGS__ \
                            ); } while (false)

//! Use this macro to log a ERROR message if the minimal logging severity
//! is at least ERROR (see `SetMinimalLogSeverity`).
//! \param _log_id_ Identifier of the component from which the message is
//!                 being logged. This parameter must be of type `const char*`
//!                 or be convertible to it.
//! \param _format_ Plain C string literal giving the format of the logged
//!                 message (see `fmt` library and Hobgoblin.Format).
//! \param ...      [optional] variables to be formatted into the logged message
//!                 as described by the _format_ parameter.
#define HG_LOG_ERROR(_log_id_, _format_, ...) \
    do{ UHOBGOBLIN_LOG_IMPL(::jbatnozic::hobgoblin::log::Severity::Error, \
                            _log_id_, \
                            _format_, \
                            __VA_ARGS__ \
                            ); } while (false)

//! Use this macro to log a FATAL message if the minimal logging severity
//! is at least FATAL (see `SetMinimalLogSeverity`).
//! \param _log_id_ Identifier of the component from which the message is
//!                 being logged. This parameter must be of type `const char*`
//!                 or be convertible to it.
//! \param _format_ Plain C string literal giving the format of the logged
//!                 message (see `fmt` library and Hobgoblin.Format).
//! \param ...      [optional] variables to be formatted into the logged message
//!                 as described by the _format_ parameter.
#define HG_LOG_FATAL(_log_id_, _format_, ...) \
    do{ UHOBGOBLIN_LOG_IMPL(::jbatnozic::hobgoblin::log::Severity::Fatal, \
                            _log_id_, \
                            _format_, \
                            __VA_ARGS__ \
                            ); } while (false)

} // namespace log
HOBGOBLIN_NAMESPACE_END
#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_LOGGING_USER_MACROS_HPP
