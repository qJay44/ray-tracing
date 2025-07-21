#pragma once

#define RT_MATERIAL_FLAG_CHECKERED_PATTERN 1u

struct RayTracingMaterial {
  vec4 color = vec4(1.f);
  alignas(16) vec3 emissionColor = vec3(0.f);
  alignas(16) vec3 specularColor = vec3(1.f);
  float emissionStrength = 0.f;
  float smoothness = 0.f;
  float specularProbability = 0.f;
  u32 flags = 0;
};

