#pragma once

#include "../engine/UBO.hpp"
#include "../engine/Shader.hpp"
#include "Sphere.hpp"
#include "Triangle.hpp"
#include "MeshInfo.hpp"

// NOTE: Must match in rt.frag
#define MAX_SPHERES 6u
#define MAX_TRIANGLES 500u
#define MAX_MESHES 1u

class Scene {
public:
  static Scene scene1(int numSpheres);
  static Scene scene2(int& numMeshes);

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
};

