#pragma once

#include "RayTracingMaterial.hpp"

struct MeshInfo {
  u32 firstTriangleIndex;
  u32 numTriangles = 0;
  alignas(16) vec3 boundsMin = vec3(FLT_MAX);
  alignas(16) vec3 boundsMax = vec3(-FLT_MAX);
  RayTracingMaterial material;
};

