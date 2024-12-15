// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_COMMON_STRONG_BOOL_HPP
#define UHOBGOBLIN_COMMON_STRONG_BOOL_HPP

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

class StrongBoolImpl {
public:
    using BoolType = void (StrongBoolImpl::*)();

    void dummyFunc() {}
};

//! A bool-like type that can be used in if-statements (and similar) while preventing
//! conversion into integral and pointer types. Useful for classes such as `std::optional`
//! to allow them to be checked for validity in an if-statement (by using
//! `operator StrongBool`) where using `operator bool` would allow them to be cast into
//! integral and pointer types by accident.
//! 
//! (don't focus too much on the actual underlying type)
using StrongBool = StrongBoolImpl::BoolType;

constexpr StrongBool SBOOL_TRUE  = &StrongBoolImpl::dummyFunc;
constexpr StrongBool SBOOL_FALSE = nullptr;

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_COMMON_STRONG_BOOL_HPP
