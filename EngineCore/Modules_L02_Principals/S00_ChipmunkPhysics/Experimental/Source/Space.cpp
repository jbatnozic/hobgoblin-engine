// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Alvin/Space.hpp>

#include <Hobgoblin/Alvin/Body.hpp>
#include <Hobgoblin/Alvin/Constraint.hpp>
#include <Hobgoblin/Alvin/Shape.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

///////////////////////////////////////////////////////////////////////////
// CONVENIENCE METHODS                                                   //
///////////////////////////////////////////////////////////////////////////

Body& Space::add(Body& aBody) {
    cpSpaceAddBody(SELF, aBody);
    return aBody;
}

Shape& Space::add(Shape& aShape) {
    cpSpaceAddShape(SELF, aShape);
    return aShape;
}

Constraint& Space::add(Constraint& aConstraint) {
    cpSpaceAddConstraint(SELF, aConstraint);
    return aConstraint;
}

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
