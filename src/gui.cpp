#include "gui.hpp"

#include <algorithm>
#include <cmath>

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

#define IM_RED   IM_COL32(255u, 0u  , 0u  , 255u)
#define IM_GREEN IM_COL32(0u ,  255u, 0u  , 255u)
#define IM_BLUE  IM_COL32(0u ,  0u  , 255u, 255u)
#define IM_WHITE IM_COL32(255u, 255u, 255u, 255u)

#define IM_CIRCLE_RADIUS 30.f

using namespace ImGui;

static bool collapsed = true;

Camera* cameraPtr;
Light* lightPtr;

void gui::link(Camera* ptr) { cameraPtr = ptr; }
void gui::link(Light* ptr)  { lightPtr  = ptr; }

void gui::toggle() { collapsed = !collapsed; }

void gui::draw() {
  static RunOnce a([]() {
    SetNextWindowPos({0, 0});
  });
  SetNextWindowCollapsed(collapsed);

  Begin("Settings");

  // ================== Free Camera ====================

  if (!cameraPtr) error("The free camera is not linked to gui");
  if (TreeNode("Free camera")) {
    SliderFloat("Near##2",  &cameraPtr->nearPlane, 0.01f, 1.f);
    SliderFloat("Far##2",   &cameraPtr->farPlane,  10.f , 100.f);
    SliderFloat("Speed##2", &cameraPtr->speed,     1.f  , 50.f);
    SliderFloat("FOV##2",   &cameraPtr->fov,       45.f , 179.f);

    TreePop();
  }

  // ================== Light ==========================

  if (!lightPtr) error("The light is not linked to gui");
  if (TreeNode("Light")) {
    bool update = false;
    update += DragFloat("x", &lightPtr->position.x, 0.1f);
    update += DragFloat("y", &lightPtr->position.y, 0.1f);
    update += DragFloat("z", &lightPtr->position.z, 0.1f);

    if (update)
      lightPtr->translation = glm::translate(mat4(1.f), lightPtr->position);

    ColorEdit3("Color", glm::value_ptr(lightPtr->color));

    TreePop();
  };

  // ================== Other ==========================

  if (TreeNode("Other")) {
    Checkbox("Show global axis", &global::drawGlobalAxis);

    TreePop();
  }

  End();
}

