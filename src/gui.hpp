#pragma once

#include "engine/Light.hpp"
#include "objects/RayTracingData.hpp"

struct gui {
  static void link(Camera* ptr);
  static void link(Light* ptr);
  static void link(RayTracingData* ptr);
  static void toggle();
  static void draw();
};

