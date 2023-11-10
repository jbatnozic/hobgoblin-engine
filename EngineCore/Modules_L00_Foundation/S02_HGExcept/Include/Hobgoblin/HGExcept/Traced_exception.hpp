#ifndef UHOBGOBLIN_HGEXCEPT_TRACED_EXCEPTION_HPP
#define UHOBGOBLIN_HGEXCEPT_TRACED_EXCEPTION_HPP

#include <Hobgoblin/Format.hpp>
#include <Hobgoblin/Preprocessor.hpp>

#include <stdexcept>
#include <string>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

//! TODO
class TracedException : public std::exception {
public:
    //! Constructs the TracedException.
    //! 
    //! It is recommended to use the `HG_THROW_TRACED` macro
    //! for convenience and not the constructor directly.
    TracedException(
        std::string aType,
        std::string aFunc,
        std::string aFile,
        int aLineNumber,
        int aErrorCode,
        std::string aErrorMessage,
        std::string aComment = ""
    );

    //!
    const std::string& getType() const noexcept;

    //! \returns the name of the function from which the exception was thrown.
    const std::string& getFunction() const noexcept;

    //! \returns the file from which the exception was thrown.
    const std::string& getFile() const noexcept;

    //! \returns the line number from which the exception was thrown.
    int getLineNumber() const noexcept;

    //! \returns the error code of the exception.
    int getErrorCode() const noexcept;

    //! \returns the error message of the exception.
    const std::string& getErrorMessage() const noexcept;

    //! \returns the comment of the exception (returns "<none>" if none was given).
    const std::string& getComment() const noexcept;

    //! \brief returns the stack trace indicating where the exception was thrown.
    //! 
    //! Depending on several factors, such as the Operating System, compiler and
    //! build type (Debug, Release etc.) the stack trace may or may not be available
    //! at runtime.
    const std::vector<std::string>& getStackTrace() const noexcept;

    //!
    const std::string& getDescription() const noexcept;

    //!
    std::string getFormattedDescription() const noexcept;

    //!
    std::string getFullFormattedDescription() const noexcept;

    //!
    const char* what() const noexcept override;

private:
    std::string _type;
    std::string _func;
    std::string _file;
    int _lineNumber;
    int _errorCode;
    std::string _errorMessage;
    std::string _comment;
    std::string _description; //!< Need to have this as member so what() would work.
    std::vector<std::string> _stackTrace;
};

#include <Hobgoblin/HGExcept/Private/Throw_traced_macros.inl>

//! TODO(add desc.)
//! (TYPE, CODE, (optional)MESSAGE/FORMAT, (optional)FURTHER ARGS)
#define HG_THROW_TRACED(_type_, _code_, ...) \
    UHOBGOBLIN_THROW_TRACED_MIDDLE(_type_, _code_, "", HG_PP_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

//! Same as `HG_THROW_TRACED` but with a comment.
#define HG_THROW_TRACED_C(_type_, _code_, _comment_, ...) \
    UHOBGOBLIN_THROW_TRACED_MIDDLE(_type_, _code_, _comment_, HG_PP_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_HGEXCEPT_TRACED_EXCEPTION_HPP
