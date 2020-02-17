#ifndef UHOBGOBLIN_RN_HANDLERS_HPP
#define UHOBGOBLIN_RN_HANDLERS_HPP

#include <cstdint>

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

void RN_IndexHandlers();

class RN_Node;

namespace detail {

using RN_HandlerFunc = void(*)(RN_Node&);
using RN_HandlerId = std::int16_t;

class RN_GlobalHandlerMapper {
public:
    static RN_GlobalHandlerMapper& getInstance();

private:
    RN_GlobalHandlerMapper();

    // std::vector<const char*> _handlerNames;
    // std::vector<const char*> _handlerPointers;
    // std::unordered_map<const char* -> RN_HandlerFunc> _rawMappings;
};

// TODO Simple class to install handler proxies

class RN_HandlerNameToIdCacher {
public:
    RN_HandlerNameToIdCacher(const char* handlerName)
        : _handlerName{handlerName}
        , _handlerId{0}
    {
    }

    RN_HandlerId getHandlerId() {
        return 0; // TODO
    }

private:
    const char* _handlerName;
    RN_HandlerId _handlerId;
};

} // namespace detail

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin_include/Private/pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_HANDLERS_HPP