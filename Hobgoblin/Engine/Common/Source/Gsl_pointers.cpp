
#include <Hobgoblin/Common/Gsl_pointers.hpp>
// #include <Hobgoblin/Utility/Exceptions.hpp>

#include <stdexcept>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace detail {

void ThrowBecauseNotNullNotSatisfied() {
    // TODO Resolve dependency to TracedException
    // throw util::TracedLogicError{"RequireNotNull condition not fulfilled"};
    throw std::runtime_error{"RequireNotNull condition not fulfilled"};
}

} // namespace detail
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>