#pragma once

#include <vector>

#include "MeshInfo.hpp"
#include "Triangle.hpp"

struct MeshRT {
  std::vector<Triangle> triangles;
  MeshInfo meshInfo;

  void loadOBJ(const fspath& file, bool printInfo);
};

