#ifndef UHOBGOBLIN_UTIL_SERIALIZATION_HPP
#define UHOBGOBLIN_UTIL_SERIALIZATION_HPP

#include <Hobgoblin/Utility/Packet.hpp>
#include <Hobgoblin/Utility/Any_ptr.hpp>

#include <string>
#include <unordered_map>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace util {

namespace detail {

using SerializableCreateFunc = void(*)(Packet&, AnyPtr);

class GlobalSerializableObjectMapper {
public:
    static GlobalSerializableObjectMapper& getInstance();

    void addMapping(std::string tag, SerializableCreateFunc createFunc);
    SerializableCreateFunc getMapping(const std::string& tag) const;

private:
    GlobalSerializableObjectMapper() = default;

    std::unordered_map<std::string, SerializableCreateFunc> _mapping;
};

struct StaticSerializableObjectMappingInitializer {
    explicit StaticSerializableObjectMappingInitializer(std::string tag, SerializableCreateFunc createFunc);
};

} // namespace detail

class Serializable {
protected:
    virtual void serialize(Packet& packet) const = 0;
    virtual std::string getSerializationTag() const = 0;
    friend void Serialize(Packet&, const Serializable&);
};

void Serialize(Packet& packet, const Serializable& serializable);
void Deserialize(Packet& packet, AnyPtr context);

#define HG_SERIALIZABLE_DEFINE_CREATE_FUNC() \
    void UHOBGOBLIN_SerializableCreate_##_tag_(); \
    jbatnozic::hobgoblin::util::StaticSerializableObjectMappingInitializer \
        staticSerializableObjectMappingInitializer_##_tag_{_tag_, UHOBGOBLIN_SerializableCreate_##_tag_}; \
    void UHOBGOBLIN_SerializableCreate_##_tag_()


} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_SERIALIZATION_HPP