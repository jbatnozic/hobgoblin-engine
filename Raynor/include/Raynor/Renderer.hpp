
#pragma once

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Math/Angle.hpp>
#include <SFML/Graphics.hpp>

#include <Raynor/Camera.hpp>
#include <Raynor/Scene.hpp>
#include <Raynor/Transformations.hpp>
#include <Raynor/Vector.hpp>

#include <deque>
#include <list>
#include <thread>
#include <vector>

namespace jbatnozic {
namespace raynor {

template <class taReal>
struct RenderSettings {
    using PZInteger = hg::PZInteger;

    PZInteger canvasWidth;
    PZInteger canvasHeight;

    Vec3<taReal> cameraOrigin;
    hg::math::Angle<taReal> cameraElevation{hg::math::Angle<taReal>::zero()};
    hg::math::Angle<taReal> cameraAngle{hg::math::Angle<taReal>::zero()};
    hg::math::Angle<taReal> fieldOfView{hg::math::Angle<taReal>::zero()};

    // camera FoV, camera origin, camera angles
    // skip rate %
};

template <class taResource>
struct Handle {
    ~Handle() {
        invalidate();
    }

    taResource operator*() {
        return *_iter;
    }

    void invalidate() {
        if (_valid) {
            _list.erase(_iter);
            _valid = false;
        }
    }

private:
    Handle(std::list<taResource>& aList, typename std::list<taResource>::const_iterator aIter)
        : _list{aList}, _iter{aIter}, _valid{true} {}

    template <class taReal>
    friend class Renderer;

    std::list<taResource>& _list;
    typename std::list<taResource>::const_iterator _iter;
    bool _valid;
};

template <class taReal>
class Renderer {
public:
    using Real = taReal;
    using PZInteger = hg::PZInteger;

    using ObjectList   = std::list<const SceneObject<Real>*>;
    using ObjectHandle = Handle<const SceneObject<Real>*>;

    using LightList    = std::list<const SceneLight<Real>*>;
    using LightHandle  = Handle<const SceneLight<Real>*>;

    explicit Renderer(PZInteger aCanvasWidth, PZInteger aCanvasHeight, PZInteger aWorkerCount);

    void setCanvasSize(PZInteger aCanvasWidth, PZInteger aCanvasHeight);
    void getCanvasSize(PZInteger& aCanvasWidth, PZInteger& aCanvasHeight);

    ObjectHandle addObject(const SceneObject<Real>* aObject);
    LightHandle addLight(const SceneLight<Real>* aLight);

    void render(const RenderSettings<Real>& aSettings);

    sf::Uint8* getBitmap();
    void getTexture(sf::Texture& aTexture);

private:
    struct RenderingJob {
        RenderingJob(const Vec3<taReal>* aPrimRays, sf::Uint8* aBitmapRow, const RenderSettings<Real>& aSettings)
            : primRays{aPrimRays}, bitmapRow{aBitmapRow}, settings{aSettings} {}

        const Vec3<taReal>* const primRays;
        sf::Uint8* const bitmapRow;
        const RenderSettings<Real>& settings;
    };

    static sf::Color CastRay(const Vec3<Real>& aRayOrigin,
                             const Vec3<Real>& aRayDirection,
                             const std::vector<const SceneObject<Real>*>& aSortedObjects,
                             const std::vector<const SceneLight<Real>*>& aLights);

    std::vector<const SceneObject<Real>*> MakeObjectVector() const {
        std::vector<const SceneObject<Real>*> result;
        result.reserve(_sceneObjects.size());
        for (auto& object : _sceneObjects) {
            result.push_back(object);
        }
        return result;
    }

    std::vector<const SceneLight<Real>*>  MakeLightVector() const {
        std::vector<const SceneLight<Real>*> result;
        result.reserve(_sceneLights.size());
        for (auto& light : _sceneLights) {
            result.push_back(light);
        }
        return result;
    }

    // worker threads, semaphores
    PZInteger _canvasWidth;
    PZInteger _canvasHeight;
    std::vector<sf::Uint8> _bitmap;

    ObjectList _sceneObjects;
    LightList _sceneLights;

    void workerImpl();
};

template <class taReal>
Renderer<taReal>::Renderer(PZInteger aCanvasWidth, PZInteger aCanvasHeight, PZInteger aWorkerCount)
    : _canvasWidth{aCanvasWidth}
    , _canvasHeight{aCanvasHeight}
{
    _bitmap.resize(_canvasWidth* _canvasHeight * 4);
}

template <class taReal>
void Renderer<taReal>::setCanvasSize(PZInteger aCanvasWidth, PZInteger aCanvasHeight) {
    // TODO
}

template <class taReal>
void Renderer<taReal>::getCanvasSize(PZInteger& aCanvasWidth, PZInteger& aCanvasHeight) {
    aCanvasWidth  = _canvasWidth;
    aCanvasHeight = _canvasHeight;
}

template <class taReal>
typename Renderer<taReal>::ObjectHandle Renderer<taReal>::addObject(const SceneObject<Real>* aObject) {
    _sceneObjects.push_back(aObject);
    return ObjectHandle{_sceneObjects, std::prev(_sceneObjects.end())};
}

template <class taReal>
typename Renderer<taReal>::LightHandle Renderer<taReal>::addLight(const SceneLight<Real>* aLights) {
    _sceneLights.push_back(aLights);
    return ObjectHandle{_sceneLights, std::prev(_sceneLights.end())};
}

template <class taReal>
void Renderer<taReal>::render(const RenderSettings<Real>& aSettings) {
    auto objectVec = MakeObjectVector();
    auto lightVec = MakeLightVector();

    std::sort(std::begin(objectVec), std::end(objectVec),
              [aSettings](const SceneObject<Real>* aFirst, const SceneObject<Real>* aSecond) {
                  auto boundSphere1 = aFirst->getBoundingSphere();
                  auto dist1 = (boundSphere1.centre - aSettings.cameraOrigin).magnitude() - boundSphere1.radius;

                  auto boundSphere2 = aSecond->getBoundingSphere();
                  auto dist2 = (boundSphere2.centre - aSettings.cameraOrigin).magnitude() - boundSphere2.radius;

                  if (dist1 >= Real{0} && dist2 >= Real{0}) {
                      return dist1 < dist2;
                  }
                  else if (dist1 < Real{0} && dist2 >= Real{0}) {
                      return true; // aFirst < [closer than] aSecond
                  }
                  else if (dist2 < Real{0}) {
                      return false; // aFirst > [farther than] aSecond
                  }
                  else {
                      return false; // treat them as equal
                  }
              });

    // TODO Adjust _canvasWidth and _canvasHeight if needed

    // Make camera
    auto camera = ConstructDefaultCamera<Real>(_canvasWidth, _canvasHeight, aSettings.fieldOfView);

    // Transform camera
    // TODO
    auto tMatrix = TMatrixCreate<taReal>();
    
    TMatrixSetZAxisRotation(tMatrix, aSettings.cameraAngle); TMatrixSetXAxisRotation(tMatrix, aSettings.cameraElevation);

    for (auto& primRay : camera) {
        const auto temp = Homogenize(primRay);
        const auto rotated = Multiply(temp, tMatrix);
        primRay.x = rotated[0][0];
        primRay.y = rotated[0][1];
        primRay.z = rotated[0][2];
    }

    // Create rendering jobs
    std::deque<RenderingJob> jobs;
    for (PZInteger y = 0; y < _canvasHeight; y += 1) {
        const Vec3<taReal>* primRays = camera.data() + y * _canvasWidth;
        sf::Uint8* bitmapRow = &(_bitmap[0]) + y * _canvasWidth * 4;
        jobs.emplace_back(primRays, bitmapRow, aSettings);
    }

    // Cast (TODO offload to worker threads)
    for (const auto& job : jobs) {
        for (PZInteger x = 0; x < job.settings.canvasWidth; x += 1) {

            auto primRay = job.primRays[x];
            auto color = CastRay(job.settings.cameraOrigin, primRay, objectVec, lightVec);

            sf::Uint8* pixel = job.bitmapRow + (x * 4);
            pixel[0] = color.r;
            pixel[1] = color.g;
            pixel[2] = color.b;
            pixel[3] = 0xFF; // TODO
        }
    }
}

template <class taReal>
sf::Uint8* Renderer<taReal>::getBitmap() {
    return &(_bitmap[0]);
}

template <class taReal>
sf::Color Renderer<taReal>::CastRay(const Vec3<Real>& aRayOrigin,
                                    const Vec3<Real>& aRayDirection,
                                    const std::vector<const SceneObject<Real>*>& aSortedObjects,
                                    const std::vector<const SceneLight<Real>*>& aLights) {
    const SceneObject<Real>* hitObject = nullptr;

    Real minDistance = std::numeric_limits<Real>::infinity();

    for (const auto& object : aSortedObjects) {
        taReal distance;
        if (object->intersectsRay(aRayOrigin, aRayDirection, distance)) {
            if (distance < minDistance) {
                hitObject = object;
                minDistance = distance;
                // We can break because objects are sorted by the closeness of their closest points to the camera
                // origin, so it's guaranteed that all points that the ray could potentially encounter would be
                // further away and thus discarded.
                // In retrospect it doesn't seem to work so well
                // break; TODO
            }
        }
    }

    if (hitObject != nullptr) {
        const auto pointOfIntersection = (aRayOrigin + aRayDirection * minDistance);
        Vec3<Real> normalAtIntersection;
        Vec3<Real> textureCoordinates;
        hitObject->getSurfaceData(pointOfIntersection, normalAtIntersection, textureCoordinates);

        Real colorMultiplier = 
            AngleBetween(normalAtIntersection, aRayDirection).asRadians() * hg::math::PiInverse<Real>();
        sf::Color result = hitObject->getColor();

        colorMultiplier *= colorMultiplier;
        result.r *= colorMultiplier;
        result.g *= colorMultiplier;
        result.b *= colorMultiplier;

        return result;
    }

    return sf::Color::Black; // TODO TEMP
}

} // namespace raynor
} // namespace jbatnozic