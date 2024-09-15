// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_LOGGING_USER_MACROS_HPP
#define UHOBGOBLIN_LOGGING_USER_MACROS_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Logging/Impl.hpp>
#include <Hobgoblin/Logging/Severity.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>
HOBGOBLIN_NAMESPACE_BEGIN
namespace log {

#if HG_BUILD_TYPE == HG_DEBUG
#define HG_LOG_HPDEB(_log_id_, _format_, ...) \
    do{ UHOBGOBLIN_LOG_IMPL(::jbatnozic::hobgoblin::log::Severity::Debug, \
                            _log_id_, \
                            _format_, \
                            __VA_ARGS__ \
                            ); } while (false)
#else
#define HG_LOG_HPDEB(_log_id_, _format_, ...) do {} while (false)
#endif

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

//! Use this macro to log a DEBUG message if the minimal logging severity
//! is at least DEBUG (see `SetMinimalLogSeverity`) and the program is compiled
//! in debug mode.
//!
//! \note this macro is meant for debug messages that are logged from the hot path
//!       of the program (hence the name - Hot Path DEBugging). When not in debug
//!       mode, it expands into an empty statement which doesn't log anything -
//!       regardless of the minimal logging
//!
//! \param _log_id_ Identifier of the component from which the message is
//!                 being logged. This parameter must be of type `const char*`
//!                 or be convertible to it.
//! \param _format_ Plain C string literal giving the format of the logged
//!                 message (see `fmt` library and Hobgoblin.Format).
//! \param ...      [optional] variables to be formatted into the logged message
//!                 as described by the _format_ parameter.
#define HG_LOG_HPDEB(_log_id_, _format_, ...) \
    do{ UHOBGOBLIN_LOG_IMPL(::jbatnozic::hobgoblin::log::Severity::Debug, \
                            _log_id_, \
                            _format_, \
                            __VA_ARGS__ \
                            ); } while (false)

#if HG_BUILD_TYPE != HG_DEBUG
#undef  HG_LOG_HPDEB
#define HG_LOG_HPDEB(_log_id_, _format_, ...) ((void)0)
#endif

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

// clang-format on
