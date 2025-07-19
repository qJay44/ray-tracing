#include "Scene.hpp"

#include "Room.hpp"
#include "glm/gtc/quaternion.hpp"
#include "MeshRT.hpp"
#include "utils/utils.hpp"

Scene Scene::scene1(RayTracingData& rtData) {
  constexpr vec4 palette[MAX_SPHERES] = {
    {1.00f, 0.00f, 1.00f, 1.f}, // Purple
    {0.11f, 0.11f, 0.11f, 1.f}, // Black
    {0.00f, 1.00f, 0.00f, 1.f}, // Green
    {0.00f, 1.00f, 1.00f, 1.f}, // Blue
    {1.00f, 0.00f, 0.00f, 1.f}, // Red
    {1.00f, 1.00f, 1.00f, 1.f}  // White
  };

  rtData.numSpheres = 6;
  rtData.enableEnvLight = true;

  Scene scene;
  scene.allocateSpheres();

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

  for (int i = 1; i < rtData.numSpheres; i++) {
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

Scene Scene::scene2(RayTracingData& rtData) {
  rtData.numMeshes = 1;
  rtData.enableEnvLight = true;

  MeshRT* meshesRT = new MeshRT[rtData.numMeshes];
  meshesRT[0].loadOBJ("res/obj/Knight.obj", 0.05f);

  u32 totalNumTriangles = 0;
  for (int i = 0; i < rtData.numMeshes; i++) {
    totalNumTriangles += meshesRT[i].triangles.size();

    if (totalNumTriangles > MAX_TRIANGLES)
      error("[Scene::scene2] Mesh amount of triangles [{}] exceeds the limit [{}]", totalNumTriangles, MAX_TRIANGLES);
  }

  Scene scene;
  scene.allocateTriangles();
  scene.allocateMeshes();

  // Fill trianglesBuf and meshInfosBuf
  u32 firstTriangleIndex = 0;
  for (int i = 0; i < rtData.numMeshes; i++) {
    MeshRT& mesh = meshesRT[i];
    mesh.meshInfo.firstTriangleIndex = firstTriangleIndex;

    for (size_t j = 0; j < mesh.triangles.size(); j++) {
      scene.trianglesBuf[j + firstTriangleIndex] = mesh.triangles[j];
    }

    scene.meshesInfosBuf[i] = mesh.meshInfo;

    firstTriangleIndex += mesh.triangles.size();
  }

  delete[] meshesRT;

  return scene;
}

Scene Scene::scene3(RayTracingData& rtData) {
  rtData.numMeshes = 1 + ROOM_TOTAL_MESHES; // Knight + room
  rtData.enableEnvLight = false;
  u32 totalNumTriangles = 0;

  // ===== Knight meshes ==================================== //

  MeshRT rtMeshKnight;
  rtMeshKnight.loadOBJ("res/obj/Knight.obj", 0.05f, {0.f, -10.f, 0.f});
  rtMeshKnight.rotate(PI_3, -global::up);

  totalNumTriangles += rtMeshKnight.triangles.size();

  // ===== Room meshes ====================================== //

  Room room(vec3(0.f), 20.f, 20.f, 20.f);
  const MeshRT* rtRoomMeshes = room.getMeshes();

  // ===== Preparing scene ================================== //

  totalNumTriangles += ROOM_TOTAL_TRIANGLES;

  if (totalNumTriangles > MAX_TRIANGLES)
    error("[Scene::scene2] Mesh amount of triangles [{}] exceeds the limit [{}]", totalNumTriangles, MAX_TRIANGLES);

  Scene scene;
  scene.allocateTriangles();
  scene.allocateMeshes();

  // ===== Add knight to buffers ============================ //

  u32 firstTriangleIndex = 0;
  rtMeshKnight.meshInfo.firstTriangleIndex = firstTriangleIndex;

  for (size_t i = 0; i < rtMeshKnight.triangles.size(); i++)
    scene.trianglesBuf[i + firstTriangleIndex] = rtMeshKnight.triangles[i];

  scene.meshesInfosBuf[0] = rtMeshKnight.meshInfo;
  firstTriangleIndex += rtMeshKnight.triangles.size();

  // ===== Add room to buffers ============================== //

  for (size_t i = 0; i < ROOM_TOTAL_MESHES; i++) {
    MeshRT mesh = rtRoomMeshes[i];
    mesh.meshInfo.firstTriangleIndex = firstTriangleIndex;

    for (size_t j = 0; j < mesh.triangles.size(); j++)
      scene.trianglesBuf[j + firstTriangleIndex] = mesh.triangles[j];

    scene.meshesInfosBuf[i + 1] = mesh.meshInfo;

    firstTriangleIndex += mesh.triangles.size();
  }

  return scene;
}

Scene Scene::scene4(RayTracingData& rtData) {
  rtData.numMeshes = ROOM_TOTAL_MESHES;
  rtData.numSpheres = 4;
  rtData.enableEnvLight = false;
  rtData.numRaysPerPixel = 5;
  rtData.numRayBounces = 5;

  // ===== Room meshes ====================================== //

  Room room(vec3(0.f), 50.f, 20.f, 20.f);
  room.update(ROOM_IDX_FLOOR, {vec4(1.f), vec3(0.f), 0.f, 0.f, RT_MATERIAL_FLAG_CHECKERED_PATTERN});
  room.update(ROOM_IDX_RIGHT, {{global::green, 1.f}});
  room.update(ROOM_IDX_FRONT, {{global::blue, 1.f}});
  const MeshRT* rtRoomMeshes = room.getMeshes();

  // ===== Preparing scene ================================== //

  u32 totalNumTriangles = 0;
  totalNumTriangles += ROOM_TOTAL_TRIANGLES;

  if (totalNumTriangles > MAX_TRIANGLES)
    error("[Scene::scene2] Mesh amount of triangles [{}] exceeds the limit [{}]", totalNumTriangles, MAX_TRIANGLES);

  Scene scene;
  scene.allocateSpheres();
  scene.allocateTriangles();
  scene.allocateMeshes();

  // ===== Spheres ========================================== //

  RayTracingMaterial material;
  material.color = vec4(1.f);
  material.emissionColor = vec3(0.f);
  material.emissionStrength = 0.f;

  float r = 5.f;
  vec3 initPos = vec3(0.f);
  vec3 offset = vec3(0.f);
  initPos.x -= r * rtData.numSpheres;

  for (int i = 0; i < rtData.numSpheres; i++) {
    material.smoothness = (float)i / (rtData.numSpheres - 1);

    Sphere sphere;
    sphere.pos = initPos + offset;
    sphere.radius = r;
    sphere.material = material;

    scene.spheresBuf[i] = sphere;
    offset.x += r * 2.f + 2.f;
  }

  // ===== Add room to buffers ============================== //

  u32 firstTriangleIndex = 0;
  for (int i = 0; i < rtData.numMeshes; i++) {
    MeshRT mesh = rtRoomMeshes[i];
    mesh.meshInfo.firstTriangleIndex = firstTriangleIndex;

    for (size_t j = 0; j < mesh.triangles.size(); j++)
      scene.trianglesBuf[j + firstTriangleIndex] = mesh.triangles[j];

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

void Scene::allocateSpheres() {
  GLsizeiptr size = sizeof(Sphere) * MAX_SPHERES;
  GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

  uboSpheres = UBO(1);
  uboSpheres.storage(size, flags);
  spheresBuf = (Sphere*)uboSpheres.map(size, flags);
}

void Scene::allocateTriangles() {
  GLsizeiptr size = sizeof(Triangle) * MAX_TRIANGLES;
  GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

  uboTriangles = UBO(1);
  uboTriangles.storage(size, flags);
  trianglesBuf = (Triangle*)uboTriangles.map(size, flags);
}

void Scene::allocateMeshes() {
  GLsizeiptr size = sizeof(MeshInfo) * MAX_MESHES;
  GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

  uboMeshesInfos = UBO(1);
  uboMeshesInfos.storage(size, flags);
  meshesInfosBuf = (MeshInfo*)uboMeshesInfos.map(size, flags);
}

