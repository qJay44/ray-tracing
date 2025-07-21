#pragma once

#include "../engine/Shader.hpp"
#include "RayTracingData.hpp"

// NOTE: Must match in rt.frag
#define MAX_SPHERES 6u
#define MAX_TRIANGLES 500u
#define MAX_MESHES 10u

namespace scene {
  void scene1(RayTracingData& rtData);
  void scene2(RayTracingData& rtData);
  void scene3(RayTracingData& rtData);
  void scene4(RayTracingData& rtData);

  void updateMeshBuffer(u32& firstTriIdx, MeshRT* meshes, int numMeshes, int meshIdxOffset = 0);

  void setUnifrom(const Shader& shader);
  void bind();
  void unbind();
}

