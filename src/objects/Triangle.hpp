#pragma once

#include <cmath>

#include "RayTracingMaterial.hpp"

struct Triangle {
  vec3 a;
  float pad1;
  vec3 b;
  float pad2;
  vec3 c;
  float pad3;
  vec3 normalA;
  float pad4;
  vec3 normalB;
  float pad5;
  vec3 normalC;
  float pad6;
  RayTracingMaterial material;
};

