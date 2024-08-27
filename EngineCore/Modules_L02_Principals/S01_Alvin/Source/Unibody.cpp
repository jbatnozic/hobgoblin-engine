// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Alvin/Unibody.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

Unibody::Unibody() {
    SELF.__alvinimpl_delegatePlaceholder = nullptr;
    SELF.__alvinimpl_bodyPlaceholder     = nullptr;
    SELF.__alvinimpl_shapePlaceholder    = nullptr;
}

Unibody::~Unibody() {
    if (_isInitialized) {
        SELF.shape.~Shape();
        SELF.body.~Body();
        SELF.delegate.~CollisionDelegate();
        _isInitialized = false;
    }
}

Unibody::Unibody(Unibody&& aOther) {
    if (aOther._isInitialized) {
        init(
            [&aOther]() {
                return std::move(aOther.delegate);
            },
            [&aOther]() {
                return std::move(aOther.body);
            },
            [&aOther]() {
                return std::move(aOther.shape);
            });
        aOther.~Unibody();
    } else {
        SELF.__alvinimpl_delegatePlaceholder = nullptr;
        SELF.__alvinimpl_bodyPlaceholder     = nullptr;
        SELF.__alvinimpl_shapePlaceholder    = nullptr;
    }
}

Unibody& Unibody::operator=(Unibody&& aOther) {
    if (this == &aOther) {
        return SELF;
    }

    SELF.~Unibody();

    if (aOther._isInitialized) {
        init(
            [&aOther]() {
                return std::move(aOther.delegate);
            },
            [&aOther]() {
                return std::move(aOther.body);
            },
            [&aOther]() {
                return std::move(aOther.shape);
            });
        aOther.~Unibody();
    } else {
        SELF.__alvinimpl_delegatePlaceholder = nullptr;
        SELF.__alvinimpl_bodyPlaceholder     = nullptr;
        SELF.__alvinimpl_shapePlaceholder    = nullptr;
    }

    return SELF;
}

void Unibody::addToSpace(NeverNull<cpSpace*> aSpace) {
    assert(_isInitialized);
    cpSpaceAddBody(aSpace, body);
    cpSpaceAddShape(aSpace, shape);
}

void Unibody::addToSpace(NeverNull<cpSpace*> aSpace, const math::Vector2<double>& aInitialPosition) {
    assert(_isInitialized);
    cpSpaceAddBody(aSpace, body);
    cpBodySetPosition(body, cpv(aInitialPosition.x, aInitialPosition.y));
    cpSpaceAddShape(aSpace, shape);
}

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>
