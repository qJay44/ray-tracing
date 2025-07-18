#pragma once

struct RayTracingMaterial {
  vec4 color;
  alignas(16) vec3 emissionColor; // Don't need to be aligned since it "will be combined with" emissionStrengh
  float emissionStrength;
};

