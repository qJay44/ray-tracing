#pragma once

#include "../engine/UBO.hpp"
#include "../engine/Shader.hpp"
#include "RayTracingData.hpp"
#include "Sphere.hpp"
#include "Triangle.hpp"
#include "MeshInfo.hpp"

// NOTE: Must match in rt.frag
#define MAX_SPHERES 6u
#define MAX_TRIANGLES 500u
#define MAX_MESHES 10u

class Scene {
public:
  static Scene scene1(RayTracingData& rtData);
  static Scene scene2(RayTracingData& rtData);
  static Scene scene3(RayTracingData& rtData);
  static Scene scene4(RayTracingData& rtData);

  void setUnifrom(const Shader& shader) const;
  void bind() const;
  void unbind() const;

private:
  UBO uboSpheres;
  UBO uboTriangles;
  UBO uboMeshesInfos;

  Sphere* spheresBuf = nullptr;
  Triangle* trianglesBuf = nullptr;
  MeshInfo* meshesInfosBuf = nullptr;

private:
  void allocateSpheres();
  void allocateTriangles();
  void allocateMeshes();
};

