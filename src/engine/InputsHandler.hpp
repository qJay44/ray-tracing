#pragma once

#include "Camera.hpp"

struct InputsHandler {
  InputsHandler() = delete;

  static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

  static void process(Camera* camera);
};

