#pragma once

#include <cmath>

struct Triangle {
  alignas(16) vec3 a;
  alignas(16) vec3 b;
  alignas(16) vec3 c;
  alignas(16) vec3 normalA;
  alignas(16) vec3 normalB;
  alignas(16) vec3 normalC;
};

