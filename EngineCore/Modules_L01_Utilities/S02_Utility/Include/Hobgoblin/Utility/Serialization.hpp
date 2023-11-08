#ifndef UHOBGOBLIN_UTIL_SERIALIZATION_HPP
#define UHOBGOBLIN_UTIL_SERIALIZATION_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Any_ptr.hpp>
#include <Hobgoblin/Utility/Packet.hpp>

#include <string>
#include <type_traits>
#include <unordered_map>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

class PolymorphicSerializable {
protected:
    virtual void serialize(PacketBase& packet) const = 0;
    virtual std::string getSerializableTag() const = 0;
    template <class T>
    friend void Serialize(PacketBase&, const T&);
};

using DeserializeMethod = void(*)(PacketBase&, AnyPtr, int);

namespace detail {

// TODO (potential) optimize map keys (std::String -> const char*)
class GlobalSerializableRegistry {
public:
    static GlobalSerializableRegistry& getInstance();

    void registerClass(std::string tag, DeserializeMethod deserializeMethod);
    DeserializeMethod getDeserializeMethod(const std::string& tag) const;

private:
    GlobalSerializableRegistry() = default;
    std::unordered_map<std::string, DeserializeMethod> _deserializeMethods;
};

} // namespace detail

template <class T>
void Serialize(PacketBase& packet, const T& serializable) {
    Packet intermediaryPacket;
    serializable.serialize(intermediaryPacket);

    if constexpr (std::is_base_of<PolymorphicSerializable, T>::value) {
        packet << serializable.getSerializableTag();
    }
    else {
        packet << T::SERIALIZABLE_TAG;
    }

    packet << static_cast<std::uint32_t>(intermediaryPacket.getDataSize());
    packet.append(intermediaryPacket.getData(), intermediaryPacket.getDataSize());
}

template <class T>
void RegisterSerializable() {
    auto& mapper = detail::GlobalSerializableRegistry::getInstance();
    mapper.registerClass(T::SERIALIZABLE_TAG, (DeserializeMethod)T::deserialize);
}

void Deserialize(Packet& packet, AnyPtr context, int contextTag = 0);

class DeserializationError : public TracedException {
public:
    using TracedException::TracedException;
};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_SERIALIZATION_HPP
