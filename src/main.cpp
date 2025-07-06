#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <direct.h>

#include "engine/UBO.hpp"
#include "engine/mesh/meshes.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "engine/Camera.hpp"
#include "GLFW/glfw3.h"
#include "engine/Shader.hpp"
#include "engine/InputsHandler.hpp"
#include "engine/CameraStorage.hpp"
#include "engine/Light.hpp"
#include "engine/FBO.hpp"
#include "engine/RBO.hpp"
#include "global.hpp"
#include "gui.hpp"
#include "objects/Sphere.hpp"
#include "utils/clrp.hpp"

#define MAX_SPHERES 5u

using global::window;

Camera* CameraStorage::scene = nullptr;
Camera* CameraStorage::helper1 = nullptr;

void GLAPIENTRY MessageCallback(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam
) {
  if (source == GL_DEBUG_SOURCE_SHADER_COMPILER) return; // Shader error (have other message callback)

  clrp::clrp_t clrpError;
  clrpError.attr = clrp::ATTRIBUTE::BOLD;
  clrpError.fg = clrp::FG::RED;
  fprintf(
    stderr, "GL CALLBACK: %s source = 0x%x, id = 0x%x type = 0x%x, severity = 0x%x, message = %s\n",
    (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), source, id, type, severity, clrp::format(message, clrpError).c_str()
  );
  exit(1);
}

int main() {
  // Assuming the executable is launching from its own directory
  _chdir("../../../src");
  srand(static_cast<unsigned int>(time(nullptr)));

  // GLFW init
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

  // Window init
  window = glfwCreateWindow(1200, 720, "Sphere", NULL, NULL);
  ivec2 winSize;
  glfwGetWindowSize(window, &winSize.x, &winSize.y);
  dvec2 winCenter = dvec2(winSize) / 2.;

  if (!window) {
    printf("Failed to create GFLW window\n");
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetCursorPos(window, winCenter.x, winSize.y * 0.5f);

  // GLAD init
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    return EXIT_FAILURE;
  }

  glViewport(0, 0, winSize.x, winSize.y);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

  // ===== Shaders ============================================== //

  Shader::setDirectoryLocation("shaders");
  Shader::setDefaultShader(SHADER_DEFAULT_TYPE_COLOR_SHADER, "default/color.vert", "default/color.frag");
  Shader::setDefaultShader(SHADER_DEFAULT_TYPE_NORMALS_SHADER, "default/normal.vert", "default/normal.frag", "default/normal.geom");
  Shader::setDefaultShader(SHADER_DEFAULT_TYPE_TEXTURE_SHADER, "default/texture.vert", "default/texture.frag");

  Shader mainShader("main.vert", "main.frag");
  Shader colorShader = Shader::getDefaultShader(SHADER_DEFAULT_TYPE_COLOR_SHADER);

  mainShader.setUniform2f("u_resolution", vec2(winSize));

  // ===== Light ================================================ //

  Light light(vec3{0.f, 0.f, 10.f}, 1.5f);

  // ===== Cameras ============================================== //

  Camera cameraScene({0.f, 0.f, 1.f}, {0.f, 0.f, -1.f}, 100.f);
  cameraScene.setFarPlane(100.f);
  cameraScene.setSpeed(5.f);

  Camera cameraHelper1({0.f, 0.f, -1.f}, {0.f, 0.f, 1.f}, 100.f);
  cameraHelper1.setFarPlane(100.f);
  cameraHelper1.setSpeed(5.f);

  CameraStorage::scene = &cameraScene;
  CameraStorage::helper1 = &cameraHelper1;

  // ===== Inputs Handler ======================================= //

  glfwSetScrollCallback(window, InputsHandler::scrollCallback);
  glfwSetKeyCallback(window, InputsHandler::keyCallback);

  // ===== Spheres ============================================== //

  Sphere spheres[MAX_SPHERES];
  float offset = 0.f;
  for (size_t i = 0; i < MAX_SPHERES; i++) {
    float r = (i + 1) * 0.1f;

    RayTracingMaterial material;
    material.color = {randColor255Norm(), 1.f};
    material.emissionColor = vec3(0.f);
    material.emissionStrength = 0.f;

    Sphere sphere;
    sphere.pos = {offset + r, 0.f, 0.f};
    sphere.radius = r;
    sphere.material = material;

    if (i + 1 == MAX_SPHERES) {
      sphere.material.color = vec4(1.f);
      sphere.material.emissionColor = vec3(1.f);
      sphere.material.emissionStrength = 2.f;
      sphere.pos.x -= offset * 0.5f;
      sphere.pos.y += r * 2.f;
    }

    spheres[i] = sphere;
    offset += r + r + 0.1f;
  }

  UBO ubo(1, spheres, sizeof(Sphere) * MAX_SPHERES);
  mainShader.setUniformBlock("u_spheresBlock", 0);
  ubo.bindBase(0);
  ubo.unbind();

  // ============================================================ //

  gui::link(&cameraScene);
  gui::link(&light);

  TexParams depthTexParams{
    GL_NEAREST,
    GL_NEAREST,
    GL_CLAMP_TO_EDGE,
    GL_CLAMP_TO_EDGE,
  };

  FBO fboScreen(1);
  Texture screenColorTexture(winSize, GL_RGB, GL_RGB, "u_screenColorTex", 0);
  Texture screenDepthTexture(winSize, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, "u_screenDepthTex", 1, GL_TEXTURE_2D, depthTexParams);
  RBO rboScreen(1);
  fboScreen.attach2D(GL_COLOR_ATTACHMENT0, screenColorTexture);
  fboScreen.attach2D(GL_DEPTH_ATTACHMENT, screenDepthTexture);
  fboScreen.bind();
  rboScreen.storage(GL_DEPTH24_STENCIL8, winSize);

  FBO::unbind();

  mainShader.setUniformTexture(screenColorTexture);
  mainShader.setUniformTexture(screenDepthTexture);

  Mesh<VertexPT> screenMesh = meshes::screen();

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    static Camera* camera = &cameraScene;
    static double titleTimer = glfwGetTime();
    static double prevTime = titleTimer;
    static double currTime = prevTime;

    constexpr double fpsLimit = 1. / 90.;
    currTime = glfwGetTime();
    global::dt = currTime - prevTime;

    // FPS cap
    if (global::dt < fpsLimit) continue;
    else prevTime = currTime;

    camera = global::sceneCamera ? &cameraScene : &cameraHelper1;
    global::time += global::dt;

    if (glfwGetWindowAttrib(window, GLFW_FOCUSED))
      InputsHandler::process(camera);
    else
      glfwSetCursorPos(window, winCenter.x, winCenter.y);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Update window title every 0.3 seconds
    if (currTime - titleTimer >= 0.3) {
      u16 fps = static_cast<u16>(1.f / global::dt);
      glfwSetWindowTitle(window, std::format("FPS: {} / {:.5f} ms", fps, global::dt).c_str());
      titleTimer = currTime;
    }

    fboScreen.bind();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    light.draw(camera, colorShader);

    camera->draw(cameraScene, colorShader, CAMERA_FLAG_DRAW_DIRECTIONS | CAMERA_FLAG_DRAW_RAYS);

    FBO::unbind();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    screenColorTexture.bind();
    screenDepthTexture.bind();

    ubo.bind();
    screenMesh.draw(camera, mainShader);

    screenColorTexture.unbind();
    screenDepthTexture.unbind();

    if (global::drawGlobalAxis)
      meshes::axis(50.f).draw(camera, colorShader);

    gui::draw();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();

  return 0;
}

