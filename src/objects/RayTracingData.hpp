#pragma once

#include "../engine/Shader.hpp"

struct RayTracingData {
  vec3 groundColor;
  vec3 skyHorizonColor;
  vec3 skyZenithColor;
  int numRaysPerPixel;
  float sunFocus;
  float sunIntensity;

  void update(const Shader& shader) const {
    const GLint numRenderedFramesLoc = shader.getUniformLoc("u_numRenderedFrames");
    const GLint groundColorLoc       = shader.getUniformLoc("u_groundColor");
    const GLint skyColorHorizonLoc   = shader.getUniformLoc("u_skyHorizonColor");
    const GLint skyColorZenithLoc    = shader.getUniformLoc("u_skyZenithColor");
    const GLint numRaysPerPixelLoc   = shader.getUniformLoc("u_numRaysPerPixel");
    const GLint sunFocusLoc          = shader.getUniformLoc("u_sunFocus");
    const GLint sunIntensityLoc      = shader.getUniformLoc("u_sunIntensity");

    shader.setUniform1i(numRenderedFramesLoc, global::frameId);
    shader.setUniform3f(groundColorLoc, groundColor);
    shader.setUniform3f(skyColorHorizonLoc, skyHorizonColor);
    shader.setUniform3f(skyColorZenithLoc, skyZenithColor);
    shader.setUniform1i(numRaysPerPixelLoc, numRaysPerPixel);
    shader.setUniform1f(sunFocusLoc, sunFocus);
    shader.setUniform1f(sunIntensityLoc, sunIntensity);
  }
};

