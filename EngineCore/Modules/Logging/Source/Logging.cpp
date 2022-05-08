
#include <Hobgoblin/Logging.hpp>

#include <iostream> // Temp?

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace log {
namespace {
    
} // namespace

HG_DYNAPI void HGCALL SetMinimalLogSeverity(Severity aSeverity) {
    // TODO
}

HG_DYNAPI Severity HGCALL GetMinimalLogSeverity() {
    return Severity::All;
}

namespace detail {
HG_DYNAPI void HGCALL FuncLogImpl(Severity aSeverity, const char* aLogId, const std::string& aFormattedOutput) {
    std::cout << fmt::format("[<time>] [{}] {}: {}\n",
                             static_cast<int>(aSeverity),
                             aLogId,
                             aFormattedOutput);
}
} // namespace detail

} // namespace log
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>