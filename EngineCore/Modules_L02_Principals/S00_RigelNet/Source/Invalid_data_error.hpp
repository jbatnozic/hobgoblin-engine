// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_RN_INVALID_DATA_ERROR_HPP
#define UHOBGOBLIN_RN_INVALID_DATA_ERROR_HPP

#include <Hobgoblin/HGExcept.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

//! Exception that's thrown when a piece of inconsistent or invalid data is
//! received from the remote.
//! \note use this to let the connectors gracefully end connections in case of
//!       errors and DO NOT let this exception escape to users!
class InvalidDataError : public TracedRuntimeError {
public:
    using TracedRuntimeError::TracedRuntimeError;
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_INVALID_DATA_ERROR_HPP
