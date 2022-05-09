#ifndef UHOBGOBLIN_LOGGING_USER_MACROS_HPP
#define UHOBGOBLIN_LOGGING_USER_MACROS_HPP

#include <Hobgoblin/Logging/Impl.hpp>
#include <Hobgoblin/Logging/Severity.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>
HOBGOBLIN_NAMESPACE_BEGIN
namespace log {

//! TODO(description)
#define HG_LOG_DEBUG(_log_id_, _format_, ...) \
    do{ UHOBGOBLIN_LOG_IMPL(::jbatnozic::hobgoblin::log::Severity::Debug, \
                            _log_id_, \
                            _format_, \
                            __VA_ARGS__ \
                            ); }while(false)

//! TODO(description)
#define HG_LOG_INFO(_log_id_, _format_, ...) \
    do{ UHOBGOBLIN_LOG_IMPL(::jbatnozic::hobgoblin::log::Severity::Info, \
                            _log_id_, \
                            _format_, \
                            __VA_ARGS__ \
                            ); }while(false)

//! TODO(description)
#define HG_LOG_WARN(_log_id_, _format_, ...) \
    do{ UHOBGOBLIN_LOG_IMPL(::jbatnozic::hobgoblin::log::Severity::Warning, \
                            _log_id_, \
                            _format_, \
                            __VA_ARGS__ \
                            ); }while(false)

//! TODO(description)
#define HG_LOG_ERROR(_log_id_, _format_, ...) \
    do{ UHOBGOBLIN_LOG_IMPL(::jbatnozic::hobgoblin::log::Severity::Error, \
                            _log_id_, \
                            _format_, \
                            __VA_ARGS__ \
                            ); }while(false)

//! TODO(description)
#define HG_LOG_FATAL(_log_id_, _format_, ...) \
    do{ UHOBGOBLIN_LOG_IMPL(::jbatnozic::hobgoblin::log::Severity::Fatal, \
                            _log_id_, \
                            _format_, \
                            __VA_ARGS__ \
                            ); }while(false)

} // namespace log
HOBGOBLIN_NAMESPACE_END
#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_LOGGING_USER_MACROS_HPP
