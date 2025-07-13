#include "scenes.hpp"

#include "glm/gtc/quaternion.hpp"
#include "MeshRT.hpp"
#include "utils/utils.hpp"

namespace scenes {

void scene1(Sphere* spheresBuf, UBO& uboSpheres) {
  constexpr vec4 palette[NUM_SPHERES] = {
    {1.00f, 0.00f, 1.00f, 1.f}, // Purple
    {0.11f, 0.11f, 0.11f, 1.f}, // Black
    {0.00f, 1.00f, 0.00f, 1.f}, // Green
    {0.00f, 1.00f, 1.00f, 1.f}, // Blue
    {1.00f, 0.00f, 0.00f, 1.f}, // Red
    {1.00f, 1.00f, 1.00f, 1.f}  // White
  };

  GLsizeiptr size = sizeof(Sphere) * NUM_SPHERES;
  GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

  uboSpheres.storage(size, flags);
  spheresBuf = (Sphere*)uboSpheres.map(size, flags);

  RayTracingMaterial bigSphereMaterial;
  bigSphereMaterial.color = palette[0];
  bigSphereMaterial.emissionColor = vec3(0.f);
  bigSphereMaterial.emissionStrength = 0.f;

  Sphere bigSphere;
  bigSphere.pos = vec3(0.f);
  bigSphere.radius = 10.f;
  bigSphere.material = bigSphereMaterial;

  spheresBuf[0] = bigSphere;

  vec3 spawnFromBigSphereCenterDir = global::up;
  glm::quat q = glm::angleAxis(-PI * 0.1f, global::right);
  spawnFromBigSphereCenterDir = q * spawnFromBigSphereCenterDir;

  for (size_t i = 1; i < NUM_SPHERES; i++) {
    float r = i * 0.1f;

    RayTracingMaterial material;
    material.color = palette[i];
    material.emissionColor = vec3(0.f);
    material.emissionStrength = 0.f;

    Sphere sphere;
    sphere.pos = spawnFromBigSphereCenterDir * (bigSphere.radius + r);
    sphere.radius = r;
    sphere.material = material;

    spheresBuf[i] = sphere;
    q = glm::angleAxis(PI * 0.03f, global::right);
    spawnFromBigSphereCenterDir = q * spawnFromBigSphereCenterDir;
  }
}

void scene2(Triangle* trianglesBuf, MeshInfo* meshesInfosBuf, UBO& uboTriangles, UBO& uboMeshesInfos) {
  MeshRT meshesRT[NUM_MESHES];
  meshesRT[0].loadOBJ("res/obj/Knight.obj", false);

  u32 totalNumTriangles = 0;
  for (size_t i = 0; i < NUM_MESHES; i++) {
    totalNumTriangles += meshesRT[i].triangles.size();

    if (totalNumTriangles > MAX_TRIANGLES)
      error("[scenes::scene2] Mesh amount of triangles [{}] exceeds the limit [{}]", totalNumTriangles, MAX_TRIANGLES);
  }

  // Allocate triangles for the shader
  {
    GLsizeiptr size = sizeof(Triangle) * MAX_TRIANGLES;
    GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    uboTriangles.storage(size, flags);
    trianglesBuf = (Triangle*)uboTriangles.map(size, flags);
  }

  // Allocate meshes for the shader
  {
    GLsizeiptr size = sizeof(MeshInfo) * NUM_MESHES;
    GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    uboMeshesInfos.storage(size, flags);
    meshesInfosBuf = (MeshInfo*)uboMeshesInfos.map(size, flags);
  }

  // Fill trianglesBuf and meshInfosBuf
  u32 firstTriangleIndex = 0;
  for (size_t i = 0; i < NUM_MESHES; i++) {
    MeshRT& mesh = meshesRT[i];
    mesh.meshInfo.firstTriangleIndex = firstTriangleIndex;

    for (size_t j = 0; j < mesh.triangles.size(); j++) {
      trianglesBuf[j + firstTriangleIndex] = mesh.triangles[j];
    }

    meshesInfosBuf[i] = mesh.meshInfo;

    firstTriangleIndex += mesh.triangles.size();
  }
}

} // namespace scenes

