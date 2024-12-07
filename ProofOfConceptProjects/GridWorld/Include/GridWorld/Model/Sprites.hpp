#pragma once

#include <Hobgoblin/Graphics/Sprite_loader.hpp>

#include <array>
#include <cstdint>
#include <memory> // for std::hash

namespace gridworld {

namespace hg = ::jbatnozic::hobgoblin;

class SpriteId {
public:
    using WrappedType = hg::gr::SpriteIdNumerical;

    SpriteId(hg::gr::SpriteIdNumerical aNumber = 0) {
        set(aNumber);
    }

    operator hg::gr::SpriteIdNumerical() const {
        return get();
    }

    void set(hg::gr::SpriteIdNumerical aNumber) {
        std::memcpy(_buffer.data(), &aNumber, sizeof(aNumber));
    }

    hg::gr::SpriteIdNumerical get() const {
        hg::gr::SpriteIdNumerical number;
        std::memcpy(&number, _buffer.data(), sizeof(number));
        return number;
    }

    bool operator==(const SpriteId& aOther) const {
        return _buffer == aOther._buffer;
    }

    //! Access the underlying storage of the instance.
    //! \warning If you are only a user of this library, you will NEVER need to call this method!
    std::array<std::uint8_t, 4>& getUnderlyingStorage() {
        return _buffer;
    }

private:
    static constexpr std::size_t BUFFER_SIZE = sizeof(hg::gr::SpriteIdNumerical);
    std::array<std::uint8_t, BUFFER_SIZE> _buffer;
};

static_assert(alignof(SpriteId) == 1);

} // namespace gridworld

template <>
struct std::hash<gridworld::SpriteId> {
    std::size_t operator()(const gridworld::SpriteId& aSpriteId) const noexcept {
        return std::hash<gridworld::SpriteId::WrappedType>{}(aSpriteId.get());
    }
};
