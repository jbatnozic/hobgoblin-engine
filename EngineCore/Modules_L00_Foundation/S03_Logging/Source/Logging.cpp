// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/Format/Chrono.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <iostream> // Temp?
#include <mutex>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace log {
namespace {
std::mutex gLoggingMutex;
Severity gMinimalLoggingSeverity = Severity::Warning;
std::vector<LoggingObserverInterface*> gLoggingObservers;

const char* SeverityCString(Severity aSeverity) {
    switch (aSeverity) {
    case Severity::Debug:
        return "DEBUG";

    case Severity::Info: 
        return " INFO";

    case Severity::Warning:
        return " WARN";

    case Severity::Error:
        return "ERROR";

    case Severity::Fatal:
        return "FATAL";

    default: return "  ?  ";
    }
}
} // namespace

HG_DYNAPI void HGCALL SetMinimalLogSeverity(Severity aSeverity) {
    std::lock_guard<decltype(gLoggingMutex)> lock{gLoggingMutex};
    gMinimalLoggingSeverity = aSeverity;
}

HG_DYNAPI Severity HGCALL GetMinimalLogSeverity() {
    std::lock_guard<decltype(gLoggingMutex)> lock{gLoggingMutex};
    return gMinimalLoggingSeverity;
}

HG_DYNAPI void HGCALL RegisterLoggingObserver(LoggingObserverInterface& aObserver) {
    std::lock_guard<decltype(gLoggingMutex)> lock{gLoggingMutex};
    gLoggingObservers.push_back(&aObserver);
}

HG_DYNAPI void HGCALL UnregisterLoggingObserver(LoggingObserverInterface& aObserver) {
    std::lock_guard<decltype(gLoggingMutex)> lock{gLoggingMutex};
    auto iter = std::remove_if(gLoggingObservers.begin(), gLoggingObservers.end(),
                               [&](LoggingObserverInterface* aObserverInCollection) {
                                   return aObserverInCollection == &aObserver;
                               });
    gLoggingObservers.erase(iter, gLoggingObservers.end());
}

namespace detail {
HG_DYNAPI void HGCALL FuncLogImpl(
    Severity aSeverity, 
    const char* aLogId,
    const char* aFilePath,
    int aLineNumber, 
    const std::string& aMessage
) {
    using namespace std::chrono;

    const auto now = system_clock::now();

    const auto now_onlySecondsAsDouble =
        std::fmod(
            duration_cast<duration<double>>(now.time_since_epoch()).count(), 
            60.0);

    const std::filesystem::path filePath{aFilePath};

    const auto fullFormattedOutput = 
        fmt::format("[{:%Y-%m-%d %H:%M}:{:09.6f}] [{}] {}@{}:{}: {}",
                    now,
                    now_onlySecondsAsDouble,
                    SeverityCString(aSeverity),
                    aLogId,
                    filePath.filename().c_str(),
                    aLineNumber,
                    aMessage);

    if (aSeverity < Severity::Error) {
        std::fprintf(stdout, "%s\n", fullFormattedOutput.c_str());
    }
    else {
        std::fprintf(stderr, "%s\n", fullFormattedOutput.c_str());
    }

    // Notify observers
    {
        std::lock_guard<decltype(gLoggingMutex)> lock{gLoggingMutex};
        for (auto& observer : gLoggingObservers) {
            observer->onLog(aSeverity, aLogId, aMessage, fullFormattedOutput);
        }
    }
}
} // namespace detail

} // namespace log
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
