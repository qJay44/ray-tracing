#pragma once

#include "RayTracingMaterial.hpp"

class Sphere {
public:
  struct UniformBlock {
    vec3 pos;                    // 12 bytes for position
    float pad1 = 0.f;            // 4 bytes for padding (std140 rule; vec3 is aligned to vec4)
    float radius;                // 4 bytes for radius
    vec3 pad2 = {0.f, 0.f, 0.f}; // 12 bytes for padding (std140 rule; any member with a base alignment of 16 must be placed at an offset that is a multiple of 16)
    vec4 materialColor;          // 16 bytes for vec4
  };

  Sphere() = default;
  Sphere(vec3 position, float radius, RayTracingMaterial material);

  UniformBlock getUniformBlock() const;

private:
  vec3 position;
  float radius;
  RayTracingMaterial material;
};

