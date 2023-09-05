
#include <Hobgoblin/Window/Video_mode.hpp>

#include <SFML/Window/VideoMode.hpp>

#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace win {

VideoMode::VideoMode() 
    : width{0}
    , height{0}
    , bitsPerPixel{0}
{
}

VideoMode::VideoMode(PZInteger aModeWidth, PZInteger aModeHeight, PZInteger aModeBitsPerPixel)
    : width{aModeWidth}
    , height{aModeHeight}
    , bitsPerPixel{aModeBitsPerPixel}
{
}

VideoMode VideoMode::getDesktopMode() {
    return ToHg(sf::VideoMode::getDesktopMode());
}

const std::vector<VideoMode>& VideoMode::getFullscreenModes() {
    struct Holder {
        Holder() {
            const auto sfVideoModes = sf::VideoMode::getFullscreenModes();
            videoModes.reserve(sfVideoModes.size());
            for (const auto& mode : sfVideoModes) {
                videoModes.push_back(ToHg(mode));
            }
        }

        std::vector<VideoMode> videoModes;
    };

    static Holder holder{};

    return holder.videoModes;
}

bool VideoMode::isValid() const {
    return ToSf(SELF).isValid();
}

bool operator==(const VideoMode& aLhs, const VideoMode& aRhs) {
    return (aLhs.width        == aRhs.width)        &&
           (aLhs.height       == aRhs.height)       &&
           (aLhs.bitsPerPixel == aRhs.bitsPerPixel);
}

bool operator!=(const VideoMode& aLhs, const VideoMode& aRhs) {
    return !(aLhs == aRhs);
}

bool operator<(const VideoMode& aLhs, const VideoMode& aRhs) {
    if (aLhs.bitsPerPixel == aRhs.bitsPerPixel) {
        if (aLhs.width == aRhs.width) {
            return aLhs.height < aRhs.height;
        }
        else {
            return aLhs.width < aRhs.width;
        }
    }
    else {
        return aLhs.bitsPerPixel < aRhs.bitsPerPixel;
    }
}

bool operator>(const VideoMode& aLhs, const VideoMode& aRhs) {
    return aRhs < aLhs;
}

bool operator<=(const VideoMode& aLhs, const VideoMode& aRhs) {
    return !(aRhs < aLhs);
}

bool operator>=(const VideoMode& aLhs, const VideoMode& aRhs) {
    return !(aLhs < aRhs);
}

} // namespace win
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
