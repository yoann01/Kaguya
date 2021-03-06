#pragma once
#include <embree2/rtcore.h>

#include "Core/Kaguya.h"
#include "Geometry/Primitive.h"

namespace Kaguya
{

class Scene
{
public:
    Scene(std::shared_ptr<Camera> camera,
          std::vector<std::shared_ptr<Primitive>> prims
                = std::vector<std::shared_ptr<Primitive>>(),
          std::vector<std::shared_ptr<Light>> lights
                = std::vector<std::shared_ptr<Light>>());
    ~Scene();

    void commitScene();

    void addPrimitive(std::shared_ptr<Primitive> &prim)
    {
        mPrims.push_back(prim);
        buildGeometry(prim.get());
    }

    void addLight(std::shared_ptr<Light> &light)
    {
        mLights.push_back(light);
    }

    bool intersect(Ray &inRay,
                   DifferentialGeometry* dg,
                   Float* tHit,
                   Float* rayEpsilon) const;

    RenderBufferTrait getRenderBuffer(uint32_t geomID) const;
    size_t getPrimitiveCount() const
    {
        return mPrims.size();
    }
    size_t getLightCount() const
    {
        return mLights.size();
    }

private:
    void buildGeometry(const Primitive* prim);

    void buildUserGeomtry(const Primitive* prim);
    void buildPolygonalMesh(const PolyMesh* prim);
    void buildSubdivisionMesh(const SubdMesh* prim);
    void buildCurve(const Curve* prim);
    void buildInstance(const Primitive* prim);

private:
    RTCScene                                mSceneContext;

    std::shared_ptr<Camera>                 mCamera;
    std::vector<std::shared_ptr<Primitive>> mPrims;
    std::vector<std::shared_ptr<Light>>     mLights;
};

}
