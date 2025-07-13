#pragma once

#include "MeshInfo.hpp"
#include "Sphere.hpp"
#include "Triangle.hpp"
#include "../engine/UBO.hpp"

// NOTE: Must match in rt.frag
#define NUM_SPHERES 6u
#define MAX_TRIANGLES 500u
#define NUM_MESHES 1u

namespace scenes {

void scene1(Sphere* spheresBuf, UBO& uboSpheres);
void scene2(Triangle* trianglesBuf, MeshInfo* meshesInfosBuf, UBO& uboTriangles, UBO& uboMeshesInfos);

} // namespace scenes

