#ifndef UHOBGOBLIN_LOGGING_IMPL_HPP
#define UHOBGOBLIN_LOGGING_IMPL_HPP

#include <Hobgoblin/Format/Format.hpp>
#include <Hobgoblin/Logging/Severity.hpp>
#include <Hobgoblin/Preprocessor.hpp>

#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace log {

namespace detail {
HG_DYNAPI void HGCALL FuncLogImpl(Severity aSeverity, const char* aLogId, const std::string& aFormattedOutput);
} // namespace detail

#define UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, ...) \
    if (_severity_ >= ::jbatnozic::hobgoblin::log::GetMinimalLogSeverity()) { \
        std::string uhobgoblin_formattedOutput = ::fmt::format(FMT_STRING(_format_), __VA_ARGS__); \
        ::jbatnozic::hobgoblin::log::detail::FuncLogImpl(_severity_, _log_id_, uhobgoblin_formattedOutput); \
    }

#define UHOBGOBLIN_LOG_IMPL_0(_severity_, _log_id_, _format_, ...) \
    if (_severity_ >= ::jbatnozic::hobgoblin::log::GetMinimalLogSeverity()) { \
        std::string uhobgoblin_formattedOutput = _format_; \
        ::jbatnozic::hobgoblin::log::detail::FuncLogImpl(_severity_, _log_id_, uhobgoblin_formattedOutput); \
    }

#define UHOBGOBLIN_LOG_IMPL_1(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_2(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_3(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_4(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_5(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_6(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_7(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_8(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_9(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_10(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)

#define UHOBGOBLIN_LOG_IMPL_11(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_12(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_13(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_14(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_15(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_16(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_17(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_18(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_19(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)
#define UHOBGOBLIN_LOG_IMPL_20(_severity_, _log_id_, _format_, ...) UHOBGOBLIN_LOG_IMPL_N(_severity_, _log_id_, _format_, __VA_ARGS__)

#if defined(UHOBGOBLIN_USING_TRADITIONAL_MSVC_PREPROCESSOR)
#   define UHOBGOBLIN_LOG_IMPL_FINAL(_num_, _severity_, _log_id_, _format_, ...) \
        UHOBGOBLIN_PP_EXPAND(UHOBGOBLIN_LOG_IMPL_##_num_(_severity_, _log_id_, _format_, __VA_ARGS__))
#else
#   define UHOBGOBLIN_LOG_IMPL_FINAL(_num_, _severity_, _log_id_, _format_, ...) \
        UHOBGOBLIN_LOG_IMPL_##_num_(_severity_, _log_id_, _format_, __VA_ARGS__)
#endif

#define UHOBGOBLIN_LOG_IMPL_MIDDLE(_num_, _severity_, _log_id_, _format_, ...) \
    UHOBGOBLIN_LOG_IMPL_FINAL(_num_, _severity_, _log_id_, _format_, __VA_ARGS__)

#define UHOBGOBLIN_LOG_IMPL(_severity_, _log_id_, _format_, ...) \
    UHOBGOBLIN_LOG_IMPL_MIDDLE(HG_PP_COUNT_ARGS(__VA_ARGS__), _severity_, _log_id_, _format_, __VA_ARGS__)

} // namespace log
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_LOGGING_IMPL_HPP