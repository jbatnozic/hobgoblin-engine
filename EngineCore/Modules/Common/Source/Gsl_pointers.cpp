
#include <Hobgoblin/Common/Gsl_pointers.hpp>
#include <Hobgoblin/Common/Traced_exceptions.hpp>

#include <stdexcept>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace detail {

void ThrowBecauseNotNullNotSatisfied() {
    throw TracedLogicError{"RequireNotNull condition not fulfilled"};
}

} // namespace detail
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
