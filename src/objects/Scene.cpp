#include "Scene.hpp"

#include "glm/gtc/quaternion.hpp"
#include "MeshRT.hpp"
#include "utils/utils.hpp"

Scene Scene::scene1(int numSpheres) {
  constexpr vec4 palette[MAX_SPHERES] = {
    {1.00f, 0.00f, 1.00f, 1.f}, // Purple
    {0.11f, 0.11f, 0.11f, 1.f}, // Black
    {0.00f, 1.00f, 0.00f, 1.f}, // Green
    {0.00f, 1.00f, 1.00f, 1.f}, // Blue
    {1.00f, 0.00f, 0.00f, 1.f}, // Red
    {1.00f, 1.00f, 1.00f, 1.f}  // White
  };

  if ((size_t)numSpheres > MAX_SPHERES)
    error("[Scene::scene1] The number of spheres [{}] exceeds the limit [{}]", numSpheres, MAX_SPHERES);

  Scene scene;

  GLsizeiptr size = sizeof(Sphere) * MAX_SPHERES;
  GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

  scene.uboSpheres = UBO(1);
  scene.uboSpheres.storage(size, flags);
  scene.spheresBuf = (Sphere*)scene.uboSpheres.map(size, flags);

  RayTracingMaterial bigSphereMaterial;
  bigSphereMaterial.color = palette[0];
  bigSphereMaterial.emissionColor = vec3(0.f);
  bigSphereMaterial.emissionStrength = 0.f;

  Sphere bigSphere;
  bigSphere.pos = vec3(0.f);
  bigSphere.radius = 10.f;
  bigSphere.material = bigSphereMaterial;

  scene.spheresBuf[0] = bigSphere;

  vec3 spawnFromBigSphereCenterDir = global::up;
  glm::quat q = glm::angleAxis(-PI * 0.1f, global::right);
  spawnFromBigSphereCenterDir = q * spawnFromBigSphereCenterDir;

  for (int i = 1; i < numSpheres; i++) {
    float r = i * 0.1f;

    RayTracingMaterial material;
    material.color = palette[i];
    material.emissionColor = vec3(0.f);
    material.emissionStrength = 0.f;

    Sphere sphere;
    sphere.pos = spawnFromBigSphereCenterDir * (bigSphere.radius + r);
    sphere.radius = r;
    sphere.material = material;

    scene.spheresBuf[i] = sphere;
    q = glm::angleAxis(PI * 0.03f, global::right);
    spawnFromBigSphereCenterDir = q * spawnFromBigSphereCenterDir;
  }

  return scene;
}

Scene Scene::scene2(int& numMeshes) {
  MeshRT meshesRT[MAX_MESHES];
  meshesRT[0].loadOBJ("res/obj/Knight.obj", 0.05f);
  numMeshes = 1;

  u32 totalNumTriangles = 0;
  for (size_t i = 0; i < MAX_MESHES; i++) {
    totalNumTriangles += meshesRT[i].triangles.size();

    if (totalNumTriangles > MAX_TRIANGLES)
      error("[scenes::scene2] Mesh amount of triangles [{}] exceeds the limit [{}]", totalNumTriangles, MAX_TRIANGLES);
  }

  Scene scene;

  // Allocate triangles for the shader
  {
    GLsizeiptr size = sizeof(Triangle) * MAX_TRIANGLES;
    GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    scene.uboTriangles = UBO(1);
    scene.uboTriangles.storage(size, flags);
    scene.trianglesBuf = (Triangle*)scene.uboTriangles.map(size, flags);
  }

  // Allocate meshes for the shader
  {
    GLsizeiptr size = sizeof(MeshInfo) * MAX_MESHES;
    GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    scene.uboMeshesInfos = UBO(1);
    scene.uboMeshesInfos.storage(size, flags);
    scene.meshesInfosBuf = (MeshInfo*)scene.uboMeshesInfos.map(size, flags);
  }

  // Fill trianglesBuf and meshInfosBuf
  u32 firstTriangleIndex = 0;
  for (int i = 0; i < numMeshes; i++) {
    MeshRT& mesh = meshesRT[i];
    mesh.meshInfo.firstTriangleIndex = firstTriangleIndex;

    for (size_t j = 0; j < mesh.triangles.size(); j++) {
      scene.trianglesBuf[j + firstTriangleIndex] = mesh.triangles[j];
    }

    scene.meshesInfosBuf[i] = mesh.meshInfo;

    firstTriangleIndex += mesh.triangles.size();
  }

  return scene;
}

void Scene::setUnifrom(const Shader& shader) const {
  static const GLint spheresBlockLoc     = shader.getUniformBlockIndex("u_spheresBlock");
  static const GLint trianglesBlockLoc   = shader.getUniformBlockIndex("u_trianglesBlock");
  static const GLint meshesInfosBlockLoc = shader.getUniformBlockIndex("u_meshesInfosBlock");

  shader.setUniformBlock(spheresBlockLoc, 0);
  shader.setUniformBlock(trianglesBlockLoc, 1);
  shader.setUniformBlock(meshesInfosBlockLoc, 2);

  uboSpheres.bindBase(0);
  uboTriangles.bindBase(1);
  uboMeshesInfos.bindBase(2);
}

void Scene::bind() const {
  uboSpheres.bind();
  uboTriangles.bind();
  uboMeshesInfos.bind();
}

void Scene::unbind() const {
  uboSpheres.unbind();
  uboTriangles.unbind();
  uboMeshesInfos.unbind();
}

