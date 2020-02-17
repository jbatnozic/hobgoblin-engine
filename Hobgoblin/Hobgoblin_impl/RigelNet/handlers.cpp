
#include <Hobgoblin_include/RigelNet/handlers.hpp>

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

void RN_IndexHandlers() {
    // TODO Impl
}

namespace detail {

RN_GlobalHandlerMapper::RN_GlobalHandlerMapper() {}

RN_GlobalHandlerMapper& RN_GlobalHandlerMapper::getInstance() {
    static RN_GlobalHandlerMapper singletonInstance{};
    return singletonInstance;
}

} // namespace detail

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin_include/Private/pmacro_undef.hpp>