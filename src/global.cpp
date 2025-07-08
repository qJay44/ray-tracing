#include "global.hpp"

namespace global {

GLFWwindow* window = nullptr;

float dt = 0.f;
float time = 0.f;

int frameId = 1;

bool guiFocused     = false;
bool sceneCamera    = true;
bool drawWireframe  = false;
bool drawNormals    = false;
bool drawGlobalAxis = false;

}// global

