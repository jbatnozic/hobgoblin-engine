// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_SPACE_HPP
#define UHOBGOBLIN_ALVIN_SPACE_HPP

#include <Hobgoblin/Alvin/Private/Memory_management.hpp>
#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>

#include <Hobgoblin/Alvin/Collision_delegate.hpp>
#include <Hobgoblin/Alvin/Query_info.hpp>

#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

class Body;
class Shape;
class Constraint;

//! TODO(description)
class Space {
public:
    Space()
        : _space{cpSpaceNew()} {}

    Space(NeverNull<cpSpace*> aSpace)
        : _space{aSpace} {}

    //! Prevent copying.
    Space(const Space&)            = delete;
    Space& operator=(const Space&) = delete;

    //! Allow cheap moving.
    Space(Space&&)            = default;
    Space& operator=(Space&&) = default;

    //! Automatic conversion to `cpSpace*` for
    //! compatibility with regular Chipmunk functions.
    //! \note the returned pointer will never be NULL
    //!       unless the object was moved from.
    operator cpSpace*() {
        return _space.get();
    }

    //! Automatic conversion to `const cpSpace*` for
    //! compatibility with regular Chipmunk functions.
    //! \note the returned pointer will never be NULL
    //!       unless the object was moved from.
    operator const cpSpace*() const {
        return _space.get();
    }

    ///////////////////////////////////////////////////////////////////////////
    // CONVENIENCE METHODS                                                   //
    ///////////////////////////////////////////////////////////////////////////

    NeverNull<cpBody*> add(NeverNull<cpBody*> aBody) {
        cpSpaceAddBody(SELF, aBody);
        return aBody;
    }

    Body& add(Body& aBody);

    NeverNull<cpShape*> add(NeverNull<cpShape*> aShape) {
        cpSpaceAddShape(SELF, aShape);
        return aShape;
    }

    Shape& add(Shape& aShape);

    NeverNull<cpConstraint*> add(NeverNull<cpConstraint*> aConstraint) {
        cpSpaceAddConstraint(SELF, aConstraint);
        return aConstraint;
    }

    Constraint& add(Constraint& aConstraint);

    void step(cpFloat aDeltaTime) {
        cpSpaceStep(SELF, aDeltaTime);
    }

    // TODO: more convenience methods

    ///////////////////////////////////////////////////////////////////////////
    // QUERIES                                                               //
    ///////////////////////////////////////////////////////////////////////////

    // callable = void(const PointQueryInfo& info)
    template <class taCallable>
    void runPointQuery(cpVect        aPoint,
                       cpFloat       aMaxDistance,
                       cpShapeFilter aShapeFilter,
                       taCallable&&  aCallable) const {
        cpSpacePointQueryFunc cpFunc =
            [](cpShape* aShape, cpVect aPoint, cpFloat aDistance, cpVect /*gradient*/, void* aData) {
                auto* delegate =
                    (aShape != nullptr) ? cpShapeGetUserData(aShape).get<CollisionDelegate>() : nullptr;
                auto* callable = static_cast<taCallable*>(aData);
                      (*callable)(PointQueryInfo{.shape        = aShape,
                                                 .delegate     = delegate,
                                                 .closestPoint = aPoint,
                                                 .distance     = aDistance});
            };
        void* const cpFuncData = std::addressof(aCallable);
        cpSpacePointQuery(_space.get(), aPoint, aMaxDistance, aShapeFilter, cpFunc, cpFuncData);
    }

    // callable = void(const PointQueryInfo& info)
    template <class taCallable>
    void runPointQuery(cpVect aPoint, cpFloat aMaxDistance, taCallable&& aCallable) const {
        runPointQuery(aPoint,
                      aMaxDistance,
                      cpShapeFilterNew(CP_NO_GROUP, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES),
                      std::forward<taCallable>(aCallable));
    }

private:
    detail::ChipmunkSpaceUPtr _space;
};

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_SPACE_HPP
