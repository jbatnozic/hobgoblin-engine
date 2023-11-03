#ifndef UHOBGOBLIN_GRAPHICS_SFML_ERR_HPP
#define UHOBGOBLIN_GRAPHICS_SFML_ERR_HPP

#include <SFML/System.hpp>

#include <sstream>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

class SFMLErrorCatcher {
public:
    SFMLErrorCatcher() {
        _previous = sf::err().rdbuf(_oss.rdbuf());
    }

    ~SFMLErrorCatcher() {
        sf::err().rdbuf(_previous);
    }

    std::string getErrorMessage() const {
        return _oss.str();
    }

private:
    std::streambuf* _previous;
    std::ostringstream _oss;
};

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !#define UHOBGOBLIN_GRAPHICS_SFML_ERR_HPP
