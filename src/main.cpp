#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <direct.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GLFW/glfw3.h"
#include "engine/Camera.hpp"
#include "engine/mesh/meshes.hpp"
#include "engine/Shader.hpp"
#include "engine/InputsHandler.hpp"
#include "engine/CameraStorage.hpp"
#include "engine/Light.hpp"
#include "engine/FBO.hpp"
#include "engine/RBO.hpp"
#include "global.hpp"
#include "gui.hpp"
#include "objects/RayTracingData.hpp"
#include "objects/scene.hpp"
#include "utils/clrp.hpp"

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
  Shader rtShader("rt.vert", "rt.frag");
  Shader averageShader("average.vert", "average.frag");
  Shader swapShader("swap.vert", "swap.frag");
  Shader colorShader = Shader::getDefaultShader(SHADER_DEFAULT_TYPE_COLOR_SHADER);

  const GLint averageNumRenderedFramesLoc = averageShader.getUniformLoc("u_numRenderedFrames");
  const GLint averageNewRenderLoc = averageShader.getUniformLoc("u_newRender");
  const GLint rtLightPosLoc = rtShader.getUniformLoc("u_lightPos");

  rtShader.setUniform2f("u_resolution", vec2(winSize));

  RayTracingData rtData;
  rtData.groundColor = vec3(0.637f);
  rtData.skyHorizonColor = {1.000f, 1.000f, 1.000f};
  rtData.skyZenithColor  = {0.289f, 0.565f, 1.000f};
  rtData.numRaysPerPixel = 1;
  rtData.numRayBounces = 2;
  rtData.sunFocus = 500.f;
  rtData.sunIntensity = 10.f;

  // ===== Light ================================================ //

  Light light(vec3{300.f, 300.f, -1000.f}, 1.5f);

  // ===== Cameras ============================================== //

  Camera cameraScene({-1.72f, 8.53f, 84.28f}, {0.00f,-0.11f, -1.03f}, 100.f);
  // Camera cameraScene({-1.85f, 10.87f, 0.71f}, {0.89f,-0.26f, -0.46f}, 100.f);
  // Camera cameraScene({-3.29f, 11.16f, 2.74f}, {0.64f, 0.02f, -0.81f}, 100.f);
  cameraScene.setFarPlane(100.f);
  cameraScene.setSpeed(20.f);

  Camera cameraHelper1({-5.f, 10.f, 5.f}, {0.48f, 0.05f, -0.91f}, 100.f);
  cameraHelper1.setFarPlane(100.f);
  cameraHelper1.setSpeed(20.f);

  CameraStorage::scene = &cameraScene;
  CameraStorage::helper1 = &cameraHelper1;

  // ===== Inputs Handler ======================================= //

  glfwSetScrollCallback(window, InputsHandler::scrollCallback);
  glfwSetKeyCallback(window, InputsHandler::keyCallback);

  // ===== Framebuffers ========================================= //

  TexParams depthTexParams{
    GL_NEAREST,
    GL_NEAREST,
    GL_CLAMP_TO_EDGE,
    GL_CLAMP_TO_EDGE,
  };

  FBO fboScreen(1);
  FBO fboRT(1);
  FBO fboAverage(1);
  FBO fboSwap(1);
  RBO rboScreen(1);

  // fboSwap write
  Texture screenColorTextureOld(winSize, GL_RGB, GL_RGB, "u_screenColorTexOld", 0);
  fboSwap.attach2D(GL_COLOR_ATTACHMENT0, screenColorTextureOld);

  // fboScreen write
  Texture screenColorTextureDefault(winSize, GL_RGB, GL_RGB, "u_screenColorTexDefault", 0);
  Texture screenDepthTexture(winSize, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, "u_screenDepthTex", 1, GL_TEXTURE_2D, depthTexParams);
  fboScreen.attach2D(GL_COLOR_ATTACHMENT0, screenColorTextureDefault);
  fboScreen.attach2D(GL_DEPTH_ATTACHMENT, screenDepthTexture);

  // fboRT write
  Texture screenColorTextureNew(winSize, GL_RGB, GL_RGB, "u_screenColorTexNew", 1); // Binding along with scscreenColorTextureOld
  fboRT.attach2D(GL_COLOR_ATTACHMENT0, screenColorTextureNew);

  // fboAverage write (swapping with old render)
  Texture screenColorTextureFinal(winSize, GL_RGB, GL_RGB, "u_screenColorTexFinal", 0);
  fboAverage.attach2D(GL_COLOR_ATTACHMENT0, screenColorTextureFinal);

  fboScreen.bind();
  rboScreen.storage(GL_DEPTH24_STENCIL8, winSize);

  FBO::unbind();

  mainShader.setUniformTexture(screenColorTextureFinal);
  rtShader.setUniformTexture(screenColorTextureNew);
  rtShader.setUniformTexture(screenDepthTexture);
  averageShader.setUniformTexture(screenColorTextureOld);
  averageShader.setUniformTexture(screenColorTextureNew);
  swapShader.setUniformTexture(screenColorTextureFinal);

  Mesh<VertexPT> screenMesh = meshes::screen();

  // ===== Scenes =============================================== //

  scene::scene4(rtData);
  scene::setUnifrom(rtShader);

  // ============================================================ //

  gui::link(&cameraScene);
  gui::link(&light);
  gui::link(&rtData);

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

    // ===== Drawing the last render to the old texture =========== //

    fboSwap.bind();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    screenColorTextureFinal.bind();
    screenMesh.draw(camera, swapShader);
    screenColorTextureFinal.unbind();

    // ===== Default world draw =================================== //

    fboScreen.bind();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    light.draw(camera, colorShader);

    camera->draw(cameraScene, colorShader, CAMERA_FLAG_DRAW_DIRECTIONS | CAMERA_FLAG_DRAW_RAYS);

    // ===== Ray tracing (Post-process) =========================== //

    fboRT.bind();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    screenColorTextureDefault.bind();
    scene::bind();

    rtData.update(rtShader);
    rtShader.setUniform3f(rtLightPosLoc, light.getPosition());
    screenMesh.draw(camera, rtShader);

    screenColorTextureDefault.unbind();
    scene::unbind();

    // ===== Average between old and new render (Post-process) ==== //

    fboAverage.bind();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    screenColorTextureOld.bind();
    screenColorTextureNew.bind();

    averageShader.setUniform1i(averageNumRenderedFramesLoc, global::frameId);
    averageShader.setUniform1i(averageNewRenderLoc, global::newRender);

    screenMesh.draw(camera, averageShader);

    screenColorTextureOld.unbind();
    screenColorTextureNew.unbind();

    // ===== Final draw =========================================== //

    FBO::unbind();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    mainShader.setUniformTexture(screenColorTextureFinal);

    screenColorTextureFinal.bind();
    screenMesh.draw(camera, mainShader);
    screenColorTextureFinal.unbind();

    if (global::drawGlobalAxis)
      meshes::axis(50.f).draw(camera, colorShader);

    gui::draw();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // ===== Post draw updates ==================================== //

    glfwSwapBuffers(window);
    glfwPollEvents();

    global::time += global::dt;
    global::frameId++;

    if (global::newRender)
      global::frameId = 1;
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();

  return 0;
}

