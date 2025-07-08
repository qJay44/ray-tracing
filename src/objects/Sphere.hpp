#pragma once

#include <cmath>

#include "RayTracingMaterial.hpp"

struct Sphere {
  vec3 pos;
  float radius;
  RayTracingMaterial material;

  void update() {
    pos.y += sin(global::time) * global::dt * (radius + radius);
  }
};

