#pragma once

#include "RayTracingMaterial.hpp"

struct Sphere {
  vec3 pos;
  float radius;
  RayTracingMaterial material;
};

