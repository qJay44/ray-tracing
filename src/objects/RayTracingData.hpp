#pragma once

#include "../engine/Shader.hpp"
#include "Room.hpp"

struct RayTracingData {
  vec3 groundColor = vec3(0.637f);
  vec3 skyHorizonColor = {1.000f, 1.000f, 1.000f};
  vec3 skyZenithColor  = {0.289f, 0.565f, 1.000f};
  int  numRaysPerPixel = 1;
  int  numRayBounces = 2;
  int  numSpheres = 0;
  int  numMeshes = 0;
  bool enableEnvLight = true;
  float sunFocus = 500.f;
  float sunIntensity = 10.f;
  float divergeStrength = 0.15f;
  float defocusStrength = 0.15f;
  float focusDistance = 1.f;

  Room room;

  void update(const Shader& shader) const {
    static const GLint numRenderedFramesLoc = shader.getUniformLoc("u_numRenderedFrames");
    static const GLint groundColorLoc       = shader.getUniformLoc("u_groundColor");
    static const GLint skyColorHorizonLoc   = shader.getUniformLoc("u_skyHorizonColor");
    static const GLint skyColorZenithLoc    = shader.getUniformLoc("u_skyZenithColor");
    static const GLint numRaysPerPixelLoc   = shader.getUniformLoc("u_numRaysPerPixel");
    static const GLint numRayBouncesLoc     = shader.getUniformLoc("u_numRayBounces");
    static const GLint numSpheresLoc        = shader.getUniformLoc("u_numSpheres");
    static const GLint numMeshesLoc         = shader.getUniformLoc("u_numMeshes");
    static const GLint enableEnvLightLoc    = shader.getUniformLoc("u_enableEnvironmentalLight");
    static const GLint sunFocusLoc          = shader.getUniformLoc("u_sunFocus");
    static const GLint sunIntensityLoc      = shader.getUniformLoc("u_sunIntensity");
    static const GLint divergeStrengthLoc   = shader.getUniformLoc("u_divergeStrength");
    static const GLint defocusStrengthLoc   = shader.getUniformLoc("u_defocusStrength");
    static const GLint focusDistanceLoc   = shader.getUniformLoc("u_focusDistance");

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
    shader.setUniform1f(divergeStrengthLoc, divergeStrength);
    shader.setUniform1f(defocusStrengthLoc, defocusStrength);
    shader.setUniform1f(focusDistanceLoc, focusDistance);
  }
};

