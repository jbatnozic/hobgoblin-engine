#ifndef UHOBGOBLIN_RN_REMOTE_OBJECT_MAPPER_HPP
#define UHOBGOBLIN_RN_REMOTE_OBJECT_MAPPER_HPP

#include <cstdint>
#include <unordered_map>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

using RN_RemoteId = std::uint32_t;

class RN_RemoteObjectMapper {
public:
    // TODO
private:
    std::unordered_map<RN_RemoteId, void*> _objectMap;
    RN_RemoteId _idCounter;
    // TODO
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_REMOTE_OBJECT_MAPPER_HPP