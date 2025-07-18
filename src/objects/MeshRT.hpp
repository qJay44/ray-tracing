#pragma once

#include <vector>

#include "MeshInfo.hpp"
#include "Triangle.hpp"

struct MeshRT {
  std::vector<Triangle> triangles;
  MeshInfo meshInfo;

  void loadOBJ(const fspath& file, float scale = 1.f, const vec3& offset = vec3(0.f), bool printInfo = false);
  void createQuad(const vec3& bottomLeft, const vec3& axisY, const vec3& axisX, const vec3& normal, const vec2& size, const RayTracingMaterial& material);

  void rotate(float rad, const vec3& axis);
};

