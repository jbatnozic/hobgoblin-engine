// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_HGEXCEPT_TRACED_EXCEPTION_HPP
#define UHOBGOBLIN_HGEXCEPT_TRACED_EXCEPTION_HPP

#include <Hobgoblin/Format.hpp>
#include <Hobgoblin/Preprocessor.hpp>

#include <stdexcept>
#include <string>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

//! A generic base exception type that's meant to capture much more
//! information about the place it was thrown from than the standard
//! `std::exception`.
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

    //! Returns a verbose single-line description of the exception, which will include
    //! all available information (type, file and line of throwing, message, code etc.),
    //! except the stack trace.
    const std::string& getDescription() const noexcept;

    //! Returns a nicely formatted multi-line description of the exception, which will include
    //! all available information (type, file and line of throwing, message, code etc.),
    //! except the stack trace.
    std::string getFormattedDescription() const noexcept;

    //! Returns a nicely formatted multi-line description of the exception, which will include
    //! all available information (type, file and line of throwing, message, code etc.),
    //! including the stack trace.
    std::string getFullFormattedDescription() const noexcept;

    //! Same as `getDescription()`, provided with compatibility with `std::exception`.
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

//! Use this macro to throw a `TracedException` or a derived exception type
//! with the same constructor parameters.
//! 
//! This macro can be used in three forms:
//!  1. HG_THROW_TRACED(<type>, <error-code>);
//!     (in this case the error message of the thrown exception will be '<no message provided>'.)
//! 
//!     For example: HG_THROW_TRACED(TracedLogicError, 1);
//! 
//!  2. HG_THROW_TRACED(<type>, <error-code>, <message>);
//!     (<message> must be a `std::string` or convertible to it.)
//! 
//!     For example: HG_THROW_TRACED(TracedLogicError, 1, "Some error description.");
//! 
//!  3. HG_THROW_TRACED(<type>, <error-code>, <format-string>, <arg0>, <arg1>, ...);
//!     (<format-string> must be a string literal and a valid format string (as per Hobgoblin/Format))
//! 
//!     For example: HG_THROW_TRACED(TracedLogicError, 1, "Expected {} but got {}.", aA, aB);
//! 
//! In all cases, the macro expands into a throw expression, with aType, aFunc,
//! aFile and aLineNumber parameters already filled in.
#define HG_THROW_TRACED(_type_, _code_, ...) \
    UHOBGOBLIN_THROW_TRACED_MIDDLE(_type_, _code_, "", HG_PP_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

//! Use this macro to throw a `TracedException` or a derived exception type
//! with the same constructor parameters.
//! The difference between this macro and `HG_THROW_TRACED` is that with this one
//! you can also provide a comment about the error (see aComment parameter of
//! `TracedException` constructor).
//! 
//! This macro can be used in three forms:
//!  1. HG_THROW_TRACED(<type>, <error-code>, <comment>);
//!     (in this case the error message of the thrown exception will be '<no message provided>'.)
//! 
//!     For example: HG_THROW_TRACED(TracedLogicError, 1);
//! 
//!  2. HG_THROW_TRACED(<type>, <error-code>, <comment>, <message>);
//!     (<message> must be a `std::string` or convertible to it.)
//! 
//!     For example: HG_THROW_TRACED(TracedLogicError, 1, "Some error description.");
//! 
//!  3. HG_THROW_TRACED(<type>, <error-code>, <comment>, <format-string>, <arg0>, <arg1>, ...);
//!     (<format-string> must be a string literal and a valid format string (as per Hobgoblin/Format))
//! 
//!     For example: HG_THROW_TRACED(TracedLogicError, 1, "Expected {} but got {}.", aA, aB);
//! 
//! In all cases, <comment> must be a `std::string` or convertible to it.
//! 
//! In all cases, the macro expands into a throw expression, with aType, aFunc,
//! aFile and aLineNumber parameters already filled in.
#define HG_THROW_TRACED_C(_type_, _code_, _comment_, ...) \
    UHOBGOBLIN_THROW_TRACED_MIDDLE(_type_, _code_, _comment_, HG_PP_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_HGEXCEPT_TRACED_EXCEPTION_HPP

// clang-format on
