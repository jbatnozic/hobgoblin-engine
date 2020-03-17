
#include <Hobgoblin/Utility/Serialization.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace util {

namespace detail {

GlobalSerializableObjectMapper& GlobalSerializableObjectMapper::getInstance() {
    static GlobalSerializableObjectMapper singletonInstance{};
    return singletonInstance;
}

void GlobalSerializableObjectMapper::addMapping(std::string tag, SerializableCreateFunc createFunc) {
    // TODO
    _mapping[tag] = createFunc;
}

SerializableCreateFunc GlobalSerializableObjectMapper::getMapping(const std::string& tag) const {
    // TODO
    return _mapping.at(tag);
}

StaticSerializableObjectMappingInitializer::StaticSerializableObjectMappingInitializer(std::string tag,
                                                                                       SerializableCreateFunc createFunc) {
    GlobalSerializableObjectMapper::getInstance().addMapping(tag, std::move(createFunc));
}

} // namespace detail

void Serialize(Packet& packet, const Serializable& serializable) {
    Packet intermediaryPacket;
    serializable.serialize(intermediaryPacket);

    packet << serializable.getSerializationTag();
    packet << intermediaryPacket.getDataSize(); // TODO Ensure it's std::size_t
    packet.append(intermediaryPacket.getData(), intermediaryPacket.getDataSize());
}

void Deserialize(Packet& packet, AnyPtr context) {
    // Format: tag, dataSize, data
    while (!packet.endOfPacket()) {
        const std::string tag = packet.extractOrThrow<std::string>();
        std::size_t dataSizeRemaining = packet.extractOrThrow<std::size_t>();
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

        detail::SerializableCreateFunc cf = detail::GlobalSerializableObjectMapper::getInstance().getMapping(tag);
        cf(intermediaryPacket, context);
    }
}


} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>