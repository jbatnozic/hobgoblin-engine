// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_SPACE_HPP
#define UHOBGOBLIN_ALVIN_SPACE_HPP

#include <Hobgoblin/Alvin/Private/Memory_management.hpp>
#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Math/Angle.hpp>

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
    // MARK: CONVENIENCE METHODS                                             //
    ///////////////////////////////////////////////////////////////////////////

    //! Add a body to the space.
    NeverNull<cpBody*> add(NeverNull<cpBody*> aBody) {
        cpSpaceAddBody(SELF, aBody);
        return aBody;
    }

    //! Add a body to the space.
    Body& add(Body& aBody);

    //! Add a shape to the space.
    NeverNull<cpShape*> add(NeverNull<cpShape*> aShape) {
        cpSpaceAddShape(SELF, aShape);
        return aShape;
    }

    //! Add a shape to the space.
    Shape& add(Shape& aShape);

    //! Add a constraint to the space.
    NeverNull<cpConstraint*> add(NeverNull<cpConstraint*> aConstraint) {
        cpSpaceAddConstraint(SELF, aConstraint);
        return aConstraint;
    }

    //! Add a constraint to the space.
    Constraint& add(Constraint& aConstraint);

    //! Update the space for the given time step. Using a fixed time step is highly recommended.
    //! Doing so can greatly increase the quality of the simulation.
    //!
    //! \param aDeltaTime delta time in seconds.
    void step(cpFloat aDeltaTime) {
        cpSpaceStep(SELF, aDeltaTime);
    }

    // TODO: more convenience methods

    ///////////////////////////////////////////////////////////////////////////
    // MARK: POINT QUERIES                                                   //
    ///////////////////////////////////////////////////////////////////////////

    //! Query the space to find all shapes intersecting a point or up to some distance from the point.
    //!
    //! \param aPoint coordinates of the point.
    //! \param aMaxDistance maximum distance from the point after which shapes won't be included in
    //!                     the results (it's enough for a part of a shape to lie within this
    //!                     distance for it to be included). If 0.0 is used, the point must lie
    //!                     inside a shape. Negative `maxDistance` is also allowed meaning that the
    //!                     point must be a under a certain depth within a shape to be considered a
    //!                     match.
    //! \param aShapeFilter filter which can exclude certain groups or categories of shapes,
    //!                     in the same way as it works for collision detection.
    //! \param aCallable callable object of type `void(const PointQueryInfo&)` which will be invoked
    //!                  for every shape that is close enough to the point or intersects it (except
    //!                  the filtered out shapes).
    //!
    //! \warning there are NO guarantees as to the order in which the callbacks will be invoked.
    //!
    //! \warning you must not remove any shapes or bodies from the space from within the callback
    //!          (aCallable), same as with collision handlers.
    template <class taCallable>
    void runPointQuery(cpVect        aPoint,
                       cpFloat       aMaxDistance,
                       cpShapeFilter aShapeFilter,
                       taCallable&&  aCallable);

    //! Query the space to find all shapes intersecting a point or up to some distance from the point.
    //!
    //! \param aPoint coordinates of the point.
    //! \param aMaxDistance maximum distance from the point after which shapes won't be included in
    //!                     the results (it's enough for a part of a shape to lie within this
    //!                     distance for it to be included). If 0.0 is used, the point must lie
    //!                     inside a shape. Negative `maxDistance` is also allowed meaning that the
    //!                     point must be a under a certain depth within a shape to be considered a
    //!                     match.
    //! \param aCallable callable object of type `void(const PointQueryInfo&)` which will be invoked
    //!                  for every shape that is close enough to the point or intersects it (except
    //!                  the filtered out shapes).
    //!
    //! \note this method is the same as the other one of the same name, except that this one does
    //!       no shape filtering whatsoever.
    //!
    //! \warning there are NO guarantees as to the order in which the callbacks will be invoked.
    //!
    //! \warning you must not remove any shapes or bodies from the space from within the callback
    //!          (aCallable), same as with collision handlers.
    template <class taCallable>
    void runPointQuery(cpVect aPoint, cpFloat aMaxDistance, taCallable&& aCallable);

    //! Query the space to find the shape that's closest to the given point (up to some distance).
    //!
    //! \param aPoint coordinates of the point.
    //! \param aMaxDistance maximum stance from the point after which shapes won't be included in
    //!                     the results (it's enough for a part of a shape to lie within this
    //!                     distance for it to be included). If 0.0 is used, the point must lie
    //!                     inside a shape. Negative `maxDistance` is also allowed meaning that the
    //!                     point must be a under a certain depth within a shape to be considered a
    //!                     match.
    //! \param aShapeFilter filter which can exclude certain groups or categories of shapes,
    //!                     in the same way as it works for collision detection.
    PointQueryInfo runClosestToPointQuery(cpVect        aPoint,
                                          cpFloat       aMaxDistance,
                                          cpShapeFilter aShapeFilter);

    //! Query the space to find the shape that's closest to the given point (up to some distance).
    //!
    //! \param aPoint coordinates of the point.
    //! \param aMaxDistance maximum stance from the point after which shapes won't be included in
    //!                     the results (it's enough for a part of a shape to lie within this
    //!                     distance for it to be included). If 0.0 is used, the point must lie
    //!                     inside a shape. Negative `maxDistance` is also allowed meaning that the
    //!                     point must be a under a certain depth within a shape to be considered a
    //!                     match.
    //! \param aShapeFilter filter which can exclude certain groups or categories of shapes,
    //!                     in the same way as it works for collision detection.
    //!
    //! \note this method is the same as the other one of the same name, except that this one does
    //!       no shape filtering whatsoever.
    PointQueryInfo runClosestToPointQuery(cpVect aPoint, cpFloat aMaxDistance);

    ///////////////////////////////////////////////////////////////////////////
    // MARK: RAY QUERIES                                                     //
    ///////////////////////////////////////////////////////////////////////////

    //! Cast a ray (of finite length) into the space and invoke a callable for each shape
    //! intersecting the ray.
    //!
    //! \param aRayStart position where the ray starts.
    //! \param aRayEnd position where the ray ends.
    //! \param aRadius radius of the ray. Setting the radius to a value greater than zero turns
    //!                the ray into a kind of a "pill shape" and checks which shapes fall within
    //!                it.
    //! \param aShapeFilter filter which can exclude certain groups or categories of shapes,
    //!                     in the same way as it works for collision detection.
    //! \param aCallable callable object of type `void(const RaycastQueryInfo&)` which will be
    //!                  invoked for every shape that intersects the ray (except the filtered out
    //!                  shapes).
    //!
    //! \warning there are NO guarantees as to the order in which the callbacks will be invoked,
    //!          so don't expect that you will get shapes in order of increasing distance from
    //!          the start of the ray, nor anything similar.
    //!
    //! \warning you must not remove any shapes or bodies from the space from within the callback
    //!          (aCallable), same as with collision handlers.
    template <class taCallable>
    void runRaycastQuery(cpVect        aRayStart,
                         cpVect        aRayEnd,
                         cpFloat       aRadius,
                         cpShapeFilter aShapeFilter,
                         taCallable&&  aCallable);

    //! Cast a ray (of finite length) into the space and invoke a callable for each shape
    //! intersecting the ray.
    //!
    //! \param aRayStart position where the ray starts.
    //! \param aRayEnd position where the ray ends.
    //! \param aRadius radius of the ray. Setting the radius to a value greater than zero turns
    //!                the ray into a kind of a "pill shape" and checks which shapes fall within
    //!                it.
    //! \param aCallable callable object of type `void(const RaycastQueryInfo&)` which will be
    //!                  invoked for every shape that intersects the ray (except the filtered out
    //!                  shapes).
    //!
    //! \note this method is the same as the other one of the same name, except that this one does
    //!       no shape filtering whatsoever.
    //!
    //! \warning there are NO guarantees as to the order in which the callbacks will be invoked,
    //!          so don't expect that you will get shapes in order of increasing distance from
    //!          the start of the ray, nor anything similar.
    //!
    //! \warning you must not remove any shapes or bodies from the space from within the callback
    //!          (aCallable), same as with collision handlers.
    template <class taCallable>
    void runRaycastQuery(cpVect aRayStart, cpVect aRayEnd, cpFloat aRadius, taCallable&& aCallable);

    //! Cast a ray (of finite length) into the space and invoke a callable for each shape
    //! intersecting the ray.
    //!
    //! \param aRayOrigin position where the ray starts.
    //! \param aRayDirection direction of the ray (0deg = towards positive X, 90deg = towards
    //!                      negative Y, 180deg = towards negative X, 270deg = towards
    //!                      positive Y).
    //! \param aRayLength length of the ray.
    //! \param aRadius radius of the ray. Setting the radius to a value greater than zero turns
    //!                the ray into a kind of a "pill shape" and checks which shapes fall within
    //!                it.
    //! \param aShapeFilter filter which can exclude certain groups or categories of shapes,
    //!                     in the same way as it works for collision detection.
    //! \param aCallable callable object of type `void(const RaycastQueryInfo&)` which will be
    //!                  invoked for every shape that intersects the ray (except the filtered out
    //!                  shapes).
    //!
    //! \warning there are NO guarantees as to the order in which the callbacks will be invoked,
    //!          so don't expect that you will get shapes in order of increasing distance from
    //!          the start of the ray, nor anything similar.
    //!
    //! \warning you must not remove any shapes or bodies from the space from within the callback
    //!          (aCallable), same as with collision handlers.
    template <class taCallable>
    void runDirectedRaycastQuery(cpVect        aRayOrigin,
                                 math::AngleD  aRayDirection,
                                 cpFloat       aRayLength,
                                 cpFloat       aRadius,
                                 cpShapeFilter aShapeFilter,
                                 taCallable&&  aCallable);

    //! Cast a ray (of finite length) into the space and invoke a callable for each shape
    //! intersecting the ray.
    //!
    //! \param aRayOrigin position where the ray starts.
    //! \param aRayDirection direction of the ray (0deg = towards positive X, 90deg = towards
    //!                      negative Y, 180deg = towards negative X, 270deg = towards
    //!                      positive Y).
    //! \param aRayLength length of the ray.
    //! \param aRadius radius of the ray. Setting the radius to a value greater than zero turns
    //!                the ray into a kind of a "pill shape" and checks which shapes fall within
    //!                it.
    //! \param aCallable callable object of type `void(const RaycastQueryInfo&)` which will be
    //!                  invoked for every shape that intersects the ray (except the filtered out
    //!                  shapes).
    //!
    //! \note this method is the same as the other one of the same name, except that this one does
    //!       no shape filtering whatsoever.
    //!
    //! \warning there are NO guarantees as to the order in which the callbacks will be invoked,
    //!          so don't expect that you will get shapes in order of increasing distance from
    //!          the start of the ray, nor anything similar.
    //!
    //! \warning you must not remove any shapes or bodies from the space from within the callback
    //!          (aCallable), same as with collision handlers.
    template <class taCallable>
    void runDirectedRaycastQuery(cpVect       aRayOrigin,
                                 math::AngleD aRayDirection,
                                 cpFloat      aRayLength,
                                 cpFloat      aRadius,
                                 taCallable&& aCallable);

    //! Cast a ray (of finite length) into the space and returns information about the first
    //! shape the ray hits, if any.
    //!
    //! \param aRayStart position where the ray starts.
    //! \param aRayEnd position where the ray ends.
    //! \param aRadius radius of the ray. Setting the radius to a value greater than zero turns
    //!                the ray into a kind of a "pill shape" and checks which shapes fall within
    //!                it.
    //! \param aShapeFilter filter which can exclude certain groups or categories of shapes,
    //!                     in the same way as it works for collision detection.
    RaycastQueryInfo runClosestToRaycastQuery(cpVect        aRayStart,
                                              cpVect        aRayEnd,
                                              cpFloat       aRadius,
                                              cpShapeFilter aShapeFilter);

    //! Cast a ray (of finite length) into the space and returns information about the first
    //! shape the ray hits, if any.
    //!
    //! \param aRayStart position where the ray starts.
    //! \param aRayEnd position where the ray ends.
    //! \param aRadius radius of the ray. Setting the radius to a value greater than zero turns
    //!                the ray into a kind of a "pill shape" and checks which shapes fall within
    //!                it.
    //!
    //! \note this method is the same as the other one of the same name, except that this one does
    //!       no shape filtering whatsoever.
    RaycastQueryInfo runClosestToRaycastQuery(cpVect aRayStart, cpVect aRayEnd, cpFloat aRadius);

    //! Cast a ray (of finite length) into the space and returns information about the first
    //! shape the ray hits, if any.
    //!
    //! \param aRayOrigin position where the ray starts.
    //! \param aRayDirection direction of the ray (0deg = towards positive X, 90deg = towards
    //!                      negative Y, 180deg = towards negative X, 270deg = towards
    //!                      positive Y).
    //! \param aRayLength length of the ray.
    //! \param aRadius radius of the ray. Setting the radius to a value greater than zero turns
    //!                the ray into a kind of a "pill shape" and checks which shapes fall within
    //!                it.
    //! \param aShapeFilter filter which can exclude certain groups or categories of shapes,
    //!                     in the same way as it works for collision detection.
    RaycastQueryInfo runClosestToDirectedRaycastQuery(cpVect        aRayOrigin,
                                                      math::AngleD  aRayDirection,
                                                      cpFloat       aRayLength,
                                                      cpFloat       aRadius,
                                                      cpShapeFilter aShapeFilter);

    //! Cast a ray (of finite length) into the space and returns information about the first
    //! shape the ray hits, if any.
    //!
    //! \param aRayOrigin position where the ray starts.
    //! \param aRayDirection direction of the ray (0deg = towards positive X, 90deg = towards
    //!                      negative Y, 180deg = towards negative X, 270deg = towards
    //!                      positive Y).
    //! \param aRayLength length of the ray.
    //! \param aRadius radius of the ray. Setting the radius to a value greater than zero turns
    //!                the ray into a kind of a "pill shape" and checks which shapes fall within
    //!                it.
    //!
    //! \note this method is the same as the other one of the same name, except that this one does
    //!       no shape filtering whatsoever.
    RaycastQueryInfo runClosestToDirectedRaycastQuery(cpVect       aRayOrigin,
                                                      math::AngleD aRayDirection,
                                                      cpFloat      aRayLength,
                                                      cpFloat      aRadius);

    ///////////////////////////////////////////////////////////////////////////
    // MARK: FAST BBOX QUERIES                                               //
    ///////////////////////////////////////////////////////////////////////////

    //! Query the space to find all shapes whose bounding boxes intersect the given bounding box.
    //!
    //! \warning notice that this function uses the bounding box of a shape and not the exact
    //!          shape - this can give false positives whenever the shape isn't an axis-aligned
    //!          rectangle. However, this behaviour makes this function quite fast and ideal
    //!          for quick checks as to which shapes are roughly within an area.
    //!
    //! \param aBoundingBox bounding box to check for.
    //! \param aShapeFilter filter which can exclude certain groups or categories of shapes,
    //!                     in the same way as it works for collision detection.
    //! \param aCallable callable object of type `void(const BboxQueryInfo&)` which will be invoked
    //!                  for every shape that intersects the bbox (except the filtered out shapes).
    //!
    //! \warning there are NO guarantees as to the order in which the callbacks will be invoked.
    //!
    //! \warning you must not remove any shapes or bodies from the space from within the callback
    //!          (aCallable), same as with collision handlers.
    template <class taCallable>
    void runFastBboxQuery(const cpBB& aBoundingBox, cpShapeFilter aShapeFilter, taCallable&& aCallable);

    //! Query the space to find all shapes whose bounding boxes intersect the given bounding box.
    //!
    //! \warning notice that this function uses the bounding box of a shape and not the exact
    //!          shape - this can give false positives whenever the shape isn't an axis-aligned
    //!          rectangle. However, this behaviour makes this function quite fast and ideal
    //!          for quick checks as to which shapes are roughly within an area.
    //!
    //! \param aBoundingBox bounding box to check for.
    //! \param aCallable callable object of type `void(const BboxQueryInfo&)` which will be invoked
    //!                  for every shape that intersects the bbox (except the filtered out shapes).
    //!
    //! \note this method is the same as the other one of the same name, except that this one does
    //!       no shape filtering whatsoever.
    //!
    //! \warning there are NO guarantees as to the order in which the callbacks will be invoked.
    //!
    //! \warning you must not remove any shapes or bodies from the space from within the callback
    //!          (aCallable), same as with collision handlers.
    template <class taCallable>
    void runFastBboxQuery(const cpBB& aBoundingBox, taCallable&& aCallable);

    ///////////////////////////////////////////////////////////////////////////
    // MARK: SHAPE QUERIES                                                   //
    ///////////////////////////////////////////////////////////////////////////

    //! Query the space to find all shapes that intersect the given shape.
    //!
    //! \param aShape shape to check for.
    //! \param aShapeFilter filter which can exclude certain groups or categories of shapes,
    //!                     in the same way as it works for collision detection.
    //! \param aCallable callable object of type `void(const ShapeQueryInfo&)` which will be invoked
    //!                  for every shape that intersects the shape (except the filtered out shapes).
    //!
    //! \warning there are NO guarantees as to the order in which the callbacks will be invoked.
    //!
    //! \warning you must not remove any shapes or bodies from the space from within the callback
    //!          (aCallable), same as with collision handlers.
    template <class taCallable>
    void runShapeQuery(NeverNull<cpShape*> aShape, cpShapeFilter aShapeFilter, taCallable&& aCallable);

    //! Query the space to find all shapes that intersect the given shape.
    //!
    //! \param aShape shape to check for.
    //! \param aCallable callable object of type `void(const ShapeQueryInfo&)` which will be invoked
    //!                  for every shape that intersects the shape (except the filtered out shapes).
    //!
    //! \note this method is the same as the other one of the same name, except that this one does
    //!       no shape filtering whatsoever.
    //!
    //! \warning there are NO guarantees as to the order in which the callbacks will be invoked.
    //!
    //! \warning you must not remove any shapes or bodies from the space from within the callback
    //!          (aCallable), same as with collision handlers.
    template <class taCallable>
    void runShapeQuery(NeverNull<cpShape*> aShape, taCallable&& aCallable);

private:
    detail::ChipmunkSpaceUPtr _space;
};

///////////////////////////////////////////////////////////////////////////
// MARK: INLINE IMPLEMENTATIONS                                          //
///////////////////////////////////////////////////////////////////////////

template <class taCallable>
void Space::runPointQuery(cpVect        aPoint,
                          cpFloat       aMaxDistance,
                          cpShapeFilter aShapeFilter,
                          taCallable&&  aCallable) {
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

template <class taCallable>
void Space::runPointQuery(cpVect aPoint, cpFloat aMaxDistance, taCallable&& aCallable) {
    runPointQuery(aPoint, aMaxDistance, CP_SHAPE_FILTER_ALL, std::forward<taCallable>(aCallable));
}

inline PointQueryInfo Space::runClosestToPointQuery(cpVect        aPoint,
                                                    cpFloat       aMaxDistance,
                                                    cpShapeFilter aShapeFilter) {
    cpPointQueryInfo info;
    cpShape* shape = cpSpacePointQueryNearest(_space.get(), aPoint, aMaxDistance, aShapeFilter, &info);
    if (shape == nullptr) {
        return {.shape = nullptr, .delegate = nullptr};
    }
    return {.shape        = shape,
            .delegate     = shape ? cpShapeGetUserData(shape).get<CollisionDelegate>() : nullptr,
            .closestPoint = info.point,
            .distance     = info.distance};
}

inline PointQueryInfo Space::runClosestToPointQuery(cpVect aPoint, cpFloat aMaxDistance) {
    return runClosestToPointQuery(aPoint, aMaxDistance, CP_SHAPE_FILTER_ALL);
}

template <class taCallable>
void Space::runRaycastQuery(cpVect        aRayStart,
                            cpVect        aRayEnd,
                            cpFloat       aRadius,
                            cpShapeFilter aShapeFilter,
                            taCallable&&  aCallable) {
    cpSpaceSegmentQueryFunc cpFunc =
        [](cpShape* aShape, cpVect aPoint, cpVect aNormal, cpFloat aAlpha, void* aData) {
            auto* delegate =
                (aShape != nullptr) ? cpShapeGetUserData(aShape).get<CollisionDelegate>() : nullptr;
            auto* callable = static_cast<taCallable*>(aData);
                  (*callable)(RaycastQueryInfo{.shape              = aShape,
                                               .delegate           = delegate,
                                               .closestPoint       = aPoint,
                                               .surfaceNormal      = aNormal,
                                               .normalizedDistance = aAlpha});
        };
    void* const cpFuncData = std::addressof(aCallable);
    cpSpaceSegmentQuery(_space.get(), aRayStart, aRayEnd, aRadius, aShapeFilter, cpFunc, cpFuncData);
}

template <class taCallable>
void Space::runRaycastQuery(cpVect aRayStart, cpVect aRayEnd, cpFloat aRadius, taCallable&& aCallable) {
    runRaycastQuery(aRayStart,
                    aRayEnd,
                    aRadius,
                    CP_SHAPE_FILTER_ALL,
                    std::forward<taCallable>(aCallable));
}

template <class taCallable>
void Space::runDirectedRaycastQuery(cpVect        aRayOrigin,
                                    math::AngleD  aRayDirection,
                                    cpFloat       aRayLength,
                                    cpFloat       aRadius,
                                    cpShapeFilter aShapeFilter,
                                    taCallable&&  aCallable) {
    const cpVect rayEnd = cpv(aRayOrigin.x + aRayLength * aRayDirection.cos(),
                              aRayOrigin.y - aRayLength * aRayDirection.sin());
    runRaycastQuery(aRayOrigin, rayEnd, aRadius, aShapeFilter, std::forward<taCallable>(aCallable));
}

template <class taCallable>
void Space::runDirectedRaycastQuery(cpVect       aRayOrigin,
                                    math::AngleD aRayDirection,
                                    cpFloat      aRayLength,
                                    cpFloat      aRadius,
                                    taCallable&& aCallable) {
    runDirectedRaycastQuery(aRayOrigin,
                            aRayDirection,
                            aRayLength,
                            aRadius,
                            CP_SHAPE_FILTER_ALL,
                            std::forward<taCallable>(aCallable));
}

inline RaycastQueryInfo Space::runClosestToRaycastQuery(cpVect        aRayStart,
                                                        cpVect        aRayEnd,
                                                        cpFloat       aRadius,
                                                        cpShapeFilter aShapeFilter) {
    cpSegmentQueryInfo info;
    cpShape*           shape =
        cpSpaceSegmentQueryFirst(_space.get(), aRayStart, aRayEnd, aRadius, aShapeFilter, &info);
    if (shape == nullptr) {
        return {.shape = nullptr, .delegate = nullptr};
    }
    return {.shape              = shape,
            .delegate           = shape ? cpShapeGetUserData(shape).get<CollisionDelegate>() : nullptr,
            .closestPoint       = info.point,
            .surfaceNormal      = info.normal,
            .normalizedDistance = info.alpha};
}

inline RaycastQueryInfo Space::runClosestToRaycastQuery(cpVect  aRayStart,
                                                        cpVect  aRayEnd,
                                                        cpFloat aRadius) {
    return runClosestToRaycastQuery(aRayStart, aRayEnd, aRadius, CP_SHAPE_FILTER_ALL);
}

inline RaycastQueryInfo Space::runClosestToDirectedRaycastQuery(cpVect        aRayOrigin,
                                                                math::AngleD  aRayDirection,
                                                                cpFloat       aRayLength,
                                                                cpFloat       aRadius,
                                                                cpShapeFilter aShapeFilter) {
    const cpVect rayEnd = cpv(aRayOrigin.x + aRayLength * aRayDirection.cos(),
                              aRayOrigin.y - aRayLength * aRayDirection.sin());
    return runClosestToRaycastQuery(aRayOrigin, rayEnd, aRadius, aShapeFilter);
}

inline RaycastQueryInfo Space::runClosestToDirectedRaycastQuery(cpVect       aRayOrigin,
                                                                math::AngleD aRayDirection,
                                                                cpFloat      aRayLength,
                                                                cpFloat      aRadius) {
    return runClosestToDirectedRaycastQuery(aRayOrigin,
                                            aRayDirection,
                                            aRayLength,
                                            aRadius,
                                            CP_SHAPE_FILTER_ALL);
}

template <class taCallable>
void Space::runFastBboxQuery(const cpBB&   aBoundingBox,
                             cpShapeFilter aShapeFilter,
                             taCallable&&  aCallable) {
    cpSpaceBBQueryFunc cpFunc = [](cpShape* aShape, void* aData) {
        auto* delegate =
            (aShape != nullptr) ? cpShapeGetUserData(aShape).get<CollisionDelegate>() : nullptr;
        auto* callable = static_cast<taCallable*>(aData);
              (*callable)(BboxQueryInfo{.shape = aShape, .delegate = delegate});
    };
    void* const cpFuncData = std::addressof(aCallable);
    cpSpaceBBQuery(_space.get(), aBoundingBox, aShapeFilter, cpFunc, cpFuncData);
}

template <class taCallable>
void Space::runFastBboxQuery(const cpBB& aBoundingBox, taCallable&& aCallable) {
    runFastBboxQuery(aBoundingBox, CP_SHAPE_FILTER_ALL, std::forward<taCallable>(aCallable));
}

template <class taCallable>
void Space::runShapeQuery(NeverNull<cpShape*> aShape,
                          cpShapeFilter       aShapeFilter,
                          taCallable&&        aCallable) {
    cpSpaceShapeQueryFunc cpFunc = [](cpShape* aShape, cpContactPointSet* aPoints, void* aData) {
        auto* delegate =
            (aShape != nullptr) ? cpShapeGetUserData(aShape).get<CollisionDelegate>() : nullptr;

        auto* callable = static_cast<taCallable*>(aData);

        (*callable)(ShapeQueryInfo{.shape = aShape, .delegate = delegate, .points = aPoints});
    };
    void* const cpFuncData = std::addressof(aCallable);
    cpSpaceShapeQuery(_space.get(), aShape, cpFunc, cpFuncData);
}

template <class taCallable>
void Space::runShapeQuery(NeverNull<cpShape*> aShape, taCallable&& aCallable) {
    runShapeQuery(aShape, CP_SHAPE_FILTER_ALL, std::forward<taCallable>(aCallable));
}

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_SPACE_HPP
