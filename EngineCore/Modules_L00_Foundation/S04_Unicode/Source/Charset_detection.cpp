
#include <Hobgoblin/Unicode/Charset_detection.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <unicode/ucsdet.h>

#include <cstring>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

namespace {

} // namespace

#define U_CHECK_STATUS(_status_, _msg_format_) \
    do { \
        if (!U_SUCCESS(_status_)) { \
            HG_THROW_TRACED(TracedRuntimeError, \
                            _status_, \
                            _msg_format_, \
                            u_errorName(_status_)); \
        } \
    } while (false)

class CharsetDetector::Impl {
public:
    Impl() {
        UErrorCode status = U_ZERO_ERROR;

        _ucsdet = decltype(_ucsdet){ucsdet_open(&status)};
        U_CHECK_STATUS(status, "Failed to initialize UCharsetDetector ({}).");
    }

    void setText(const void* aText, std::int32_t aTextByteCount) {
        UErrorCode status = U_ZERO_ERROR;

        if (aText != nullptr && aTextByteCount > 0) {
            ucsdet_setText(_ucsdet.get(), static_cast<const char*>(aText), aTextByteCount, &status);
            U_CHECK_STATUS(status, "Failed to set text to UCharsetDetector ({}).");
            _emptyText = false;
        }
        else {
            _emptyText = true;
        }
    }

    CharsetDetector::Result detect() const {
        if (_emptyText) {
            return {};
        }

        UErrorCode status = U_ZERO_ERROR;

        const UCharsetMatch* match = ucsdet_detect(_ucsdet.get(), &status);
        U_CHECK_STATUS(status, "Failed to detect charset ({}).");

        if (match == nullptr) {
            return {};
        }

        const auto confidence = ucsdet_getConfidence(match, &status);
        U_CHECK_STATUS(status, "Failed to get match confidence ({}).");

        #define TRY_MATCH_NAME(_match_, _confidence_, _name_, _enum_) \
            do { \
                UErrorCode status2_ = U_ZERO_ERROR; \
                const auto* name2_ = ucsdet_getName(_match_, &status2_); \
                U_CHECK_STATUS(status2_, "Failed to get match name ({})."); \
                if (std::strcmp(name2_, _name_) == 0) { return {_enum_, _confidence_}; } \
            } while (false)

        TRY_MATCH_NAME(match, confidence, "ISO-8859-1", Charset::ISO_8859_1);
        TRY_MATCH_NAME(match, confidence, "UTF-8",      Charset::UTF_8);
        TRY_MATCH_NAME(match, confidence, "UTF-16LE",   Charset::UTF_16_LE);
        TRY_MATCH_NAME(match, confidence, "UTF-16BE",   Charset::UTF_16_BE);

        return {};
    }

private:
    using UCharsetDetectorDeleter = decltype([](UCharsetDetector* aDetector) {
        if (aDetector != nullptr) {
            ucsdet_close(aDetector);
        }
    });
    std::unique_ptr<UCharsetDetector, UCharsetDetectorDeleter> _ucsdet;
    bool _emptyText = true;
};


CharsetDetector::CharsetDetector()
    : _impl{std::make_unique<Impl>()}
{
}

CharsetDetector::CharsetDetector(const void* aText, std::int32_t aTextByteCount)
    : _impl{std::make_unique<Impl>()}
{
    _impl->setText(aText, aTextByteCount);
}

CharsetDetector::~CharsetDetector() = default;

void CharsetDetector::setText(std::nullptr_t) {
    _impl->setText(nullptr, 0);
}

void CharsetDetector::setText(const void* aText, std::int32_t aTextByteCount) {
    _impl->setText(aText, aTextByteCount);
}

void CharsetDetector::setText(const std::string* aText) {
    if (aText == nullptr) {
        _impl->setText(nullptr, 0);
        return;
    }
    _impl->setText(aText->data(), static_cast<std::int32_t>(aText->size()));
}

CharsetDetector::Result CharsetDetector::detect() const {
    return _impl->detect();
}

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
