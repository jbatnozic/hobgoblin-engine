// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_COMMON_NULLABILITY_HPP
#define UHOBGOBLIN_COMMON_NULLABILITY_HPP

#include <Hobgoblin/Format.hpp>
#include <Hobgoblin/Preprocessor.hpp>

#include <stdexcept>
#include <string>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

template <class taNullable>
using NeverNull = taNullable; // TODO (proper implementation)

#define HG_NEVER_NULL(...) ::jbatnozic::hobgoblin::NeverNull<__VA_ARGS__>

template <class taNullable>
using AvoidNull = taNullable; // TODO (proper implementation)

#define HG_AVOID_NULL(...) ::jbatnozic::hobgoblin::AvoidNull<__VA_ARGS__>

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_COMMON_NULLABILITY_HPP
