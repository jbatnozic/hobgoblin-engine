// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_RMLUI_RMLUI_PREPROCESSING_HPP
#define UHOBGOBLIN_RMLUI_RMLUI_PREPROCESSING_HPP

#include <Hobgoblin/HGExcept.hpp>

#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>
HOBGOBLIN_NAMESPACE_BEGIN
namespace rml {

//! Thrown when preprocessing an .rcss file fails.
class PreprocessingError : TracedRuntimeError {
public:
    using TracedRuntimeError::TracedRuntimeError;
};

//! TODO(add description)
//! TODO(use filesystem::path)
void PreprocessRcssFile(const std::string& aFilePath); 

} // namespace rml
HOBGOBLIN_NAMESPACE_END
#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RMLUI_RMLUI_PREPROCESSING_HPP

// clang-format on
