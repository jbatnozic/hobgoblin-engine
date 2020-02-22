
#include <Hobgoblin/RigelNet/Packet.hpp>

#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

namespace {

template <class High, class Low>
std::ptrdiff_t ByteOffset(High* hi, Low* lo) {
    return  reinterpret_cast<const char*>(hi) - reinterpret_cast<const char*>(lo);
}

class SFMLVectorDataAccess {
public:

    static std::vector<char>& getData(sf::Packet* pack);

private:
    SFMLVectorDataAccess() {}
    std::vector<char> _data;
    std::size_t       _readPos;
    std::size_t       _sendPos;
    bool              _isValid;
};

std::vector<char>& SFMLVectorDataAccess::getData(sf::Packet* pack) {
    const SFMLVectorDataAccess* dummy = 0;
    const auto offset = ByteOffset(std::addressof(dummy->_data), dummy);

    auto* rv = reinterpret_cast<std::vector<char>*>(reinterpret_cast<char*>(pack) + offset);

    return *rv;
}

} // namespace

HOBGOBLIN_NAMESPACE_START
namespace rn {



} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
