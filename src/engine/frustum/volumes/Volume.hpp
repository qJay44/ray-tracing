#pragma once

#include "../Frustum.hpp"
#include "../../mesh/Mesh.hpp"

namespace frustum {

template<typename T>
struct Volume {
  virtual bool isOnOrForwardPlane(const Plane& plane) const = 0;
  virtual bool isOnFrustum(const Frustum& Frustum, const Mesh<T>& mesh) const = 0;
};

}

