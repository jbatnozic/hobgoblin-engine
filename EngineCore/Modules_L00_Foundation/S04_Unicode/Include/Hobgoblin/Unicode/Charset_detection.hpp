#ifndef UHOBGOBLIN_UNICODE_CHARSET_DETECTION_HPP
#define UHOBGOBLIN_UNICODE_CHARSET_DETECTION_HPP

#include <Hobgoblin/Unicode/Unicode_string.hpp>

#include <cstdint>
#include <memory>
#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

enum class Charset {
    Unknown = -1, //!< Unknown charset.
    ISO_8859_1,   //!< Legacy 8-bit superset of ASCII (aka ISO "Latin 1").
    UTF_8,        //!< UTF-8 encoded Unicode.
    UTF_16_LE,    //!< UTF-16 encoded Unicode (little endian).
    UTF_16_BE     //!< UTF-16 encoded Unicode (big endian).
};

class CharsetDetector {
public:
    //! Default constructor.
    //! Creates the detector with empty text.
    CharsetDetector();

    //! Constructor.
    //! \see `setText(const void* aText, std::int32_t aTextByteCount)`.
    CharsetDetector(const void* aText, std::int32_t aTextByteCount);

    //! Constructor.
    //! \see `setText(const std::string* aText)`.
    CharsetDetector(const std::string* aText);

    //! Destructor.
    ~CharsetDetector();

    struct Result {
        Charset      charset    = Charset::Unknown;
        std::int32_t confidence = 0; //!< 0..100
    };

    //! Sets empty text for the detector. While the detector has empty text,
    //! `detect()` will always return `Charset::Unknown` with a confidence of 0.
    void setText(std::nullptr_t);

    //! Sets text for the detector to work on.
    //! 
    //! \param aText pointer to the text data. If `nullptr` is passed, the function will behave
    //!              like `setText(std::nullptr_t)`.
    //! \param aTextByteCount how many bytes the text data consists of. If the number is not
    //!                       greater than 0, the function will behave like `setText(std::nullptr_t)`.
    //! 
    //! \warning The text must not be changed or deleted until the detector is destroyed or
    //!          a different text is set.
    void setText(const void* aText, std::int32_t aTextByteCount);

    //! Sets text for the detector to work on.
    //! \warning The text must not be changed or deleted until the detector is destroyed or
    //!          a different text is set.
    void setText(const std::string* aText);

    //! Returns the best found charset match for the currently set text.
    Result detect() const;

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UNICODE_CHARSET_DETECTION_HPP
