#ifndef UHOBGOBLIN_RN_HANDLERMGMT_HPP
#define UHOBGOBLIN_RN_HANDLERMGMT_HPP

#include <cstdint>
#include <unordered_map>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

void RN_IndexHandlers();

class RN_Node;

namespace detail {

using RN_HandlerFunc = void(*)(RN_Node&);
using RN_HandlerId = std::int16_t;

struct RN_CStringHash {
    std::size_t operator()(const char* key) const;
};

struct RN_CStringEquals {
    bool operator()(const char* x, const char* y) const;
};

class RN_GlobalHandlerMapper {
public:
    static RN_GlobalHandlerMapper& getInstance();

    const char* nameWithId(RN_HandlerId id) const;
    RN_HandlerFunc handlerWithId(RN_HandlerId id) const;
    RN_HandlerId idWithName(const char* name) const;

    void addMapping(const char* name, RN_HandlerFunc func);
    void index();

private:
    RN_GlobalHandlerMapper();

    std::vector<const char*> _handlerNames;
    std::vector<RN_HandlerFunc> _handlerPointers;
    std::unordered_map<const char*, RN_HandlerFunc, RN_CStringHash, RN_CStringEquals> _rawMappings;
};

///////////////////////////////////////////////////////////////////////////////

struct RN_StaticHandlerInitializer {
    RN_StaticHandlerInitializer(const char* name, RN_HandlerFunc func);
};

///////////////////////////////////////////////////////////////////////////////

class RN_HandlerNameToIdCacher {
public:
    RN_HandlerNameToIdCacher(const char* handlerName);

    RN_HandlerId getHandlerId();

private:
    const char* _handlerName;
    RN_HandlerId _handlerId;
};

} // namespace detail

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_HANDLERMGMT_HPP
