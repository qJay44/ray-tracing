#pragma once

#include "engine/Light.hpp"

struct gui {
  static void link(Camera* ptr);
  static void link(Light* ptr);
  static void toggle();
  static void draw();
};

