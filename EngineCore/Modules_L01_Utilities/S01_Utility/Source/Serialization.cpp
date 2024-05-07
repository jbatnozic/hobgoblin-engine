// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/Utility/Serialization.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

namespace detail {

GlobalSerializableRegistry& GlobalSerializableRegistry::getInstance() {
    static GlobalSerializableRegistry singletonInstance{};
    return singletonInstance;
}

void GlobalSerializableRegistry::registerClass(std::string tag, DeserializeMethod createFunc) {
    auto iter = _deserializeMethods.find(tag); 
    if (iter != _deserializeMethods.end()) {
        HG_THROW_TRACED(DeserializationError, 0, "Serializable with tag {} was already registered.", tag);
    }

    _deserializeMethods.emplace(std::make_pair(std::move(tag), createFunc));
    // [tag] = createFunc;
}

DeserializeMethod GlobalSerializableRegistry::getDeserializeMethod(const std::string& tag) const {
    auto iter = _deserializeMethods.find(tag);
    if (iter == _deserializeMethods.end()) {
        HG_THROW_TRACED(TracedLogicError, 0, "No serializable with tag {} was registered.", tag);
    }

    return (*iter).second;
}

} // namespace detail

void Deserialize(Packet& packet, AnyPtr context, int contextTag) {
  #if 0
    // Format: tag, dataSize, data
    while (!packet.endOfPacket()) {
        const std::string tag = packet.extract<std::string>();
        std::uint32_t dataSizeRemaining = packet.extract<std::uint32_t>();
        Packet intermediaryPacket;

        // TODO - Replace horrible temporary solution for moving packet data used below
        while (dataSizeRemaining >= sizeof(std::uint64_t)) {
            intermediaryPacket << packet.extract<std::uint64_t>();
            dataSizeRemaining -= sizeof(std::uint64_t);
        }
        while (dataSizeRemaining >= sizeof(std::uint8_t)) {
            intermediaryPacket << packet.extract<std::uint8_t>();
            dataSizeRemaining -= sizeof(std::uint8_t);
        }
        // TODO - Ends here

        DeserializeMethod _deserialize = detail::GlobalSerializableRegistry::getInstance().getDeserializeMethod(tag);
        _deserialize(intermediaryPacket, context, contextTag);
    }
  #endif
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
