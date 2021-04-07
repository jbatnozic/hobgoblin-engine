
#include <Hobgoblin/Utility/Exceptions.hpp>
#include <Hobgoblin/Utility/Serialization.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace util {

namespace detail {

GlobalSerializableRegistry& GlobalSerializableRegistry::getInstance() {
    static GlobalSerializableRegistry singletonInstance{};
    return singletonInstance;
}

void GlobalSerializableRegistry::registerClass(std::string tag, DeserializeMethod createFunc) {
    auto iter = _deserializeMethods.find(tag); 
    if (iter != _deserializeMethods.end()) {
        throw TracedLogicError("Serializable with tag " + tag + " was already registered!");
    }

    _deserializeMethods.emplace(std::make_pair(std::move(tag), createFunc));
    // [tag] = createFunc;
}

DeserializeMethod GlobalSerializableRegistry::getDeserializeMethod(const std::string& tag) const {
    auto iter = _deserializeMethods.find(tag);
    if (iter == _deserializeMethods.end()) {
        throw TracedLogicError("No serializable with tag " + tag + " was registered!");
    }

    return (*iter).second;
}

} // namespace detail

void Deserialize(Packet& packet, AnyPtr context, int contextTag) {
    // Format: tag, dataSize, data
    while (!packet.endOfPacket()) {
        const std::string tag = packet.extractOrThrow<std::string>();
        std::uint32_t dataSizeRemaining = packet.extractOrThrow<std::uint32_t>();
        Packet intermediaryPacket;

        // TODO - Replace horrible temporary solution for moving packet data used below
        while (dataSizeRemaining >= sizeof(std::uint64_t)) {
            intermediaryPacket << packet.extractOrThrow<std::uint64_t>();
            dataSizeRemaining -= sizeof(std::uint64_t);
        }
        while (dataSizeRemaining >= sizeof(std::uint8_t)) {
            intermediaryPacket << packet.extractOrThrow<std::uint8_t>();
            dataSizeRemaining -= sizeof(std::uint8_t);
        }
        // TODO - Ends here

        DeserializeMethod _deserialize = detail::GlobalSerializableRegistry::getInstance().getDeserializeMethod(tag);
        _deserialize(intermediaryPacket, context, contextTag);
    }
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
