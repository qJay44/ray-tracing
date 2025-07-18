#pragma once

#include "../engine/Shader.hpp"

struct RayTracingData {
  vec3 groundColor;
  vec3 skyHorizonColor;
  vec3 skyZenithColor;
  int numRaysPerPixel;
  int numRayBounces;
  int numSpheres = 0;
  int numMeshes = 0;
  bool enableEnvLight = true;
  float sunFocus;
  float sunIntensity;

  void update(const Shader& shader) const {
    const GLint numRenderedFramesLoc = shader.getUniformLoc("u_numRenderedFrames");
    const GLint groundColorLoc       = shader.getUniformLoc("u_groundColor");
    const GLint skyColorHorizonLoc   = shader.getUniformLoc("u_skyHorizonColor");
    const GLint skyColorZenithLoc    = shader.getUniformLoc("u_skyZenithColor");
    const GLint numRaysPerPixelLoc   = shader.getUniformLoc("u_numRaysPerPixel");
    const GLint numRayBouncesLoc     = shader.getUniformLoc("u_numRayBounces");
    const GLint numSpheresLoc        = shader.getUniformLoc("u_numSpheres");
    const GLint numMeshesLoc         = shader.getUniformLoc("u_numMeshes");
    const GLint enableEnvLightLoc    = shader.getUniformLoc("u_enableEnvironmentalLight");
    const GLint sunFocusLoc          = shader.getUniformLoc("u_sunFocus");
    const GLint sunIntensityLoc      = shader.getUniformLoc("u_sunIntensity");

    shader.setUniform1i(numRenderedFramesLoc, global::frameId);
    shader.setUniform3f(groundColorLoc, groundColor);
    shader.setUniform3f(skyColorHorizonLoc, skyHorizonColor);
    shader.setUniform3f(skyColorZenithLoc, skyZenithColor);
    shader.setUniform1i(numRaysPerPixelLoc, numRaysPerPixel);
    shader.setUniform1i(numRayBouncesLoc, numRayBounces);
    shader.setUniform1i(numSpheresLoc, numSpheres);
    shader.setUniform1i(numMeshesLoc, numMeshes);
    shader.setUniform1i(enableEnvLightLoc, enableEnvLight);
    shader.setUniform1f(sunFocusLoc, sunFocus);
    shader.setUniform1f(sunIntensityLoc, sunIntensity);
  }
};

