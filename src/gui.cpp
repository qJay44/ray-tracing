#include "gui.hpp"

#include <algorithm>
#include <cmath>

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "utils/utils.hpp"

#define IM_RED   IM_COL32(255u, 0u  , 0u  , 255u)
#define IM_GREEN IM_COL32(0u ,  255u, 0u  , 255u)
#define IM_BLUE  IM_COL32(0u ,  0u  , 255u, 255u)
#define IM_WHITE IM_COL32(255u, 255u, 255u, 255u)

#define IM_CIRCLE_RADIUS 30.f

using namespace ImGui;

static bool collapsed = true;

Camera* cameraPtr;
Light* lightPtr;
RayTracingData* rtDataPtr;

void gui::link(Camera* ptr)         { cameraPtr = ptr; }
void gui::link(Light* ptr)          { lightPtr  = ptr; }
void gui::link(RayTracingData* ptr) { rtDataPtr = ptr; }

void gui::toggle() { collapsed = !collapsed; }

void gui::draw() {
  static RunOnce a([]() {
    SetNextWindowPos({0, 0});
  });
  SetNextWindowCollapsed(collapsed);

  Begin("Settings");

  // ================== Camera =========================

  if (!cameraPtr) error("The free camera is not linked to gui");
  if (TreeNode("Camera")) {
    const vec3& pos = cameraPtr->position;
    const vec3& orientation = cameraPtr->orientation;

    SliderFloat("Near",  &cameraPtr->nearPlane, 0.01f, 1.f);
    SliderFloat("Far",   &cameraPtr->farPlane,  10.f , 100.f);
    SliderFloat("Speed", &cameraPtr->speed,     1.f  , 50.f);
    SliderFloat("FOV",   &cameraPtr->fov,       45.f , 179.f);

    SeparatorText("Position");
    Text("x: %.2f, y: %.2f, z: %.2f", pos.x, pos.y, pos.z);

    SeparatorText("Orientation");
    Text("x: %.2f, y: %.2f, z: %.2f", orientation.x, orientation.y, orientation.z);

    TreePop();
  }

  // ================== Ray tracing config =============

  if (!rtDataPtr) error("The ray tracing data struct is not linked to gui");
  if (TreeNode("Ray tracing config")) {
    ColorEdit3("Ground color", glm::value_ptr(rtDataPtr->groundColor));
    ColorEdit3("Sky horizon color", glm::value_ptr(rtDataPtr->skyHorizonColor));
    ColorEdit3("Sky zenith color", glm::value_ptr(rtDataPtr->skyZenithColor));
    SliderInt("Rays per pixel", &rtDataPtr->numRaysPerPixel, 1, 100);
    SliderInt("Ray bounces", &rtDataPtr->numRayBounces, 1, 100);
    SliderFloat("Sun focus", &rtDataPtr->sunFocus, -1.f, 1000.f);
    SliderFloat("Sun intensity", &rtDataPtr->sunIntensity, -1.f, 100.f);

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

