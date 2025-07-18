#pragma once

#include <cmath>

#include "RayTracingMaterial.hpp"

struct Sphere {
  alignas(16) vec3 pos;
  float radius;
  RayTracingMaterial material;

  void update() {
    pos.y += sin(global::time) * global::dt * (radius + radius);
  }
};

