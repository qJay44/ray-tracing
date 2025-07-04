#include "InputsHandler.hpp"

#include <cassert>

#include "../gui.hpp"
#include "imgui.h"
#include "CameraStorage.hpp"

using global::window;

static bool guiWasFocused;
static bool isHoldingShift = false;

void InputsHandler::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  switch (key) {
    case GLFW_KEY_R:
      if (action == GLFW_PRESS) {
        global::guiFocused = !global::guiFocused;
        guiWasFocused = true;
      }
      break;
    case GLFW_KEY_E:
      if (action == GLFW_PRESS) gui::toggle();
      break;
    case GLFW_KEY_1:
      if (action == GLFW_PRESS && !global::guiFocused)
        global::drawWireframe = !global::drawWireframe;
      break;
    case GLFW_KEY_2:
      if (action == GLFW_PRESS && !global::guiFocused)
        global::drawNormals = !global::drawNormals;
      break;
    case GLFW_KEY_F:
      if (action == GLFW_PRESS) {
        global::sceneCamera = !global::sceneCamera;
        CameraStorage::helper1->position    = CameraStorage::scene->position;
        CameraStorage::helper1->orientation = CameraStorage::scene->getForward();
      }
      break;
  }
}

void InputsHandler::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  if (global::guiFocused) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseWheelEvent(xoffset, yoffset);
  }
}

void InputsHandler::process(Camera* camera) {
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);

  glm::ivec2 winSize;
  glfwGetWindowSize(global::window, &winSize.x, &winSize.y);
  glm::dvec2 winCenter = dvec2(winSize) / 2.;

  if (!global::guiFocused) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    if (guiWasFocused) {
      glfwSetCursorPos(window, winCenter.x, winCenter.y);
      guiWasFocused = false;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera->moveForward();
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera->moveLeft();
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera->moveBack();
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera->moveRight();

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera->moveUp();
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) camera->moveDown();

    // =============== Holding shift =============== //

    static float camNormalSpeed = camera->getSpeed();
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
      if (!isHoldingShift)
        camNormalSpeed = camera->getSpeed();

      camera->setSpeed(camNormalSpeed * 5.f);
      isHoldingShift = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
      if (isHoldingShift)
        camera->setSpeed(camNormalSpeed);

      camNormalSpeed = camera->getSpeed();
      isHoldingShift = false;
    }

    // ============================================= //

    camera->update();

    glfwSetCursorPos(window, winCenter.x, winCenter.y);

  } else {
    camera->update(true);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}

