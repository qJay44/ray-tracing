#pragma once

#define RT_MATERIAL_FLAG_CHECKERED_PATTERN 1u

struct RayTracingMaterial {
  vec4 color;
  alignas(16) vec3 emissionColor = vec3(0.f);
  float emissionStrength = 0.f;
  float smoothness = 0.f;
  u32 flags = 0;
};

