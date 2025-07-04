#pragma once

#include "Volume.hpp"

namespace frustum {

template<typename T>
struct Sphere : public Volume<T> {
  static float radiusMultiplier;

  vec3 center = vec3(0.f);
  float radius = 0.f;

  Sphere(const vec3& inCenter, const float& inRadius)
    : Volume<T>{}, center(inCenter), radius(inRadius) {}

  bool isOnOrForwardPlane(const Plane& plane) const final {
    return plane.getSignedDistanceToPlane(center) > -radius;
  }

  bool isOnFrustum(const Frustum& frustum, const Mesh<T>& mesh) const final {
    mat4 model = mesh.getModel();
    vec3 globalScale = {glm::length(model[0]), glm::length(model[1]), glm::length(model[2])};
    vec3 globalCenter = model * vec4(center, 1.f);
    float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);

    Sphere globalSphere{globalCenter, radius * (maxScale * 0.5f)};

    return (
      globalSphere.isOnOrForwardPlane(frustum.leftFace) &&
      globalSphere.isOnOrForwardPlane(frustum.rightFace) &&
      globalSphere.isOnOrForwardPlane(frustum.farFace) &&
      globalSphere.isOnOrForwardPlane(frustum.nearFace) &&
      globalSphere.isOnOrForwardPlane(frustum.topFace) &&
      globalSphere.isOnOrForwardPlane(frustum.bottomFace)
    );
  }
};

}

