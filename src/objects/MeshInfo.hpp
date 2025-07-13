#pragma once

#include "RayTracingMaterial.hpp"

struct MeshInfo {
  u32 firstTriangleIndex;
  u32 numTriangles = 0;
  vec3 boundsMin{FLT_MAX, FLT_MAX, FLT_MAX};
  float pad1;
  vec3 boundsMax{FLT_MIN, FLT_MIN, FLT_MIN};
  float pad2;
  RayTracingMaterial material;
};

