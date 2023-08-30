#ifndef HOBGOBLIN_GRAPHICS_WINDOW_STYLE_HPP
#define HOBGOBLIN_GRAPHICS_WINDOW_STYLE_HPP

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

enum class WindowStyle : int {
    None       = 0,      ///< No border / title bar (this flag and all others are mutually exclusive)
    Titlebar   = 1 << 0, ///< Title bar + fixed border
    Resize     = 1 << 1, ///< Title bar + resizable border + maximize button
    Close      = 1 << 2, ///< Title bar + close button
    Fullscreen = 1 << 3, ///< Fullscreen mode (this flag and all others are mutually exclusive)

    Default = Titlebar | Resize | Close ///< Default window style
};

inline
WindowStyle operator|(WindowStyle aLhs, WindowStyle aRhs) {
    return static_cast<WindowStyle>(
        static_cast<int>(aLhs) | static_cast<int>(aRhs)
    );
}

inline
WindowStyle operator&(WindowStyle aLhs, WindowStyle aRhs) {
    return static_cast<WindowStyle>(
        static_cast<int>(aLhs) & static_cast<int>(aRhs)
    );
}

inline
WindowStyle operator^(WindowStyle aLhs, WindowStyle aRhs) {
    return static_cast<WindowStyle>(
        static_cast<int>(aLhs) ^ static_cast<int>(aRhs)
    );
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !HOBGOBLIN_GRAPHICS_WINDOW_STYLE_HPP
