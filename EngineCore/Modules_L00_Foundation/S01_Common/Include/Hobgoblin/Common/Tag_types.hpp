#ifndef UHOBGOBLIN_COMMON_TAG_TYPES_HPP
#define UHOBGOBLIN_COMMON_TAG_TYPES_HPP

#include <cstddef>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

//! Declares an empty struct in the current namespace to be used as a tag type
//! to discriminate between calls of methods that have the same names and same
//! parameters.
//! 
//! The name of the declared struct will be the passed name (`_name_`)
//! with `_Tag` appended at the end.
//! 
//! A constexpr variable with the name equal to the passed one (`_name_`)
//! will also be declared for convenience.
//! 
//! \see https://www.fluentcpp.com/2018/04/27/tag-dispatching/.
#define HG_DECLARE_TAG_TYPE(_name_) \
    constexpr struct _name_##_Tag {} _name_

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_COMMON_TAG_TYPES_HPP
