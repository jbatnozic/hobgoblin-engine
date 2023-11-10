#ifndef UHOBGOBLIN_HGEXCEPT_COMMON_EXCEPTIONS_HPP
#define UHOBGOBLIN_HGEXCEPT_COMMON_EXCEPTIONS_HPP

#include <Hobgoblin/HGExcept/Traced_exception.hpp>

#include <cassert>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////
// LOGIC ERRORS                                                          //
///////////////////////////////////////////////////////////////////////////

//! Base traced exception class to indicate conditions that would be
//!  avoided in a well-made program.
//! We don't expect to ever catch one of these, but... things happen.
class TracedLogicError : public TracedException {
public:
    using TracedException::TracedException;
};

//! Intended to be thrown when execution reaches unfinished code.
class NotImplementedError : public TracedLogicError {
public:
    using TracedLogicError::TracedLogicError;
};

//! TODO
#define HG_NOT_IMPLEMENTED(...) \
    HG_THROW_TRACED_C(::jbatnozic::hobgoblin::NotImplementedError, \
                      0, \
                      "Execution reached part of code which not implemented or not finished.", \
                      __VA_ARGS__)

//! Intended to be thrown when execution reaches code that was supposed to be unreachable.
class ReachedUnreachableCodeError : public TracedLogicError {
public:
    using TracedLogicError::TracedLogicError;
};

//! TODO
#define HG_UNREACHABLE(...) \
    HG_THROW_TRACED_C(::jbatnozic::hobgoblin::ReachedUnreachableCodeError, \
                      0, \
                      "Execution reached part of code which was supposed to be unreachable.", \
                      __VA_ARGS__)

//!
class AssertionFailedError : public TracedLogicError {
public:
    using TracedLogicError::TracedLogicError;
};

#ifdef NDEBUG // RELEASE
    #define UHOBGOBLIN_ASSERT(_expr_) do { static_cast<void>(0); } while (false)
#else // DEBUG
    #define UHOBGOBLIN_ASSERT(_expr_) \
        do { if (!(_expr_)) {  \
            HG_THROW_TRACED_C(::jbatnozic::hobgoblin::AssertionFailedError, \
                              0, \
                              "Expression was: '" #_expr_ "'.", \
                              "Debug assertion failed!"); \
        }} while (false)
#endif

//! If NDEBUG is not defined (usually in Debug mode):
//!     Checks that the given expression is true; otherwise AssertionFailedError is thrown.
//! 
//! If NDEBUG is defined (usually in Release mode):
//!     Does nothing.
//! 
//! In either case, this macro always expands into a statement which has
//! to be terminated by a semicolon.
#define HG_ASSERT(_expr_) \
    UHOBGOBLIN_ASSERT(_expr_)

//! Checks that the given expression is true; otherwise AssertionFailedError is thrown
//! (even if NDEBUG is defined!).
//! This macro always expands into a statement which has
//! to be terminated by a semicolon.
#define HG_HARD_ASSERT(_expr_) \
    do { if (!(_expr_)) {  \
        HG_THROW_TRACED_C(::jbatnozic::hobgoblin::AssertionFailedError, \
                          0, \
                          "Expression was: '" #_expr_ "'.", \
                          "Hard assertion failed!"); \
    }} while (false)

//! Intended to be thrown when an invalid argument is passed to a function.
class InvalidArgumentError : public TracedLogicError {
public:
    using TracedLogicError::TracedLogicError;
};

#define HG_VALIDATE_ARGUMENT(_expr_, ...) \
    do { if (!(_expr_)) {  \
        HG_THROW_TRACED_C(::jbatnozic::hobgoblin::InvalidArgumentError, \
                          0, \
                          "Validation expression was: '" #_expr_ "'.", \
                          __VA_ARGS__); \
    }} while (false)

//!
class PreconditionNotMetError : public TracedLogicError {
public:
    using TracedLogicError::TracedLogicError;
};

#define HG_VALIDATE_PRECONDITION(_expr_) \
    do { if (!(_expr_)) {  \
        HG_THROW_TRACED_C(::jbatnozic::hobgoblin::PreconditionNotMetError, \
                          0, \
                          "Precondition expression was: '" #_expr_ "'.", \
                          "Precondition was not met!"); \
    }} while (false)

///////////////////////////////////////////////////////////////////////////
// RUNTIME ERRORS                                                        //
///////////////////////////////////////////////////////////////////////////

//! Base traced exception class to indicate conditions only detectable at run time.
class TracedRuntimeError : public TracedException {
public:
    using TracedException::TracedException;
};

//! Traced exception class to indicate I/O errors
//! (during operations with peripherals, disks, files, networking and similar).
class IOError : public TracedRuntimeError {
public:
    using TracedRuntimeError::TracedRuntimeError;
};

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_HGEXCEPT_COMMON_EXCEPTIONS_HPP
