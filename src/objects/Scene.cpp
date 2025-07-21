#include "scene.hpp"

#include "glm/gtc/quaternion.hpp"
#include "../engine/UBO.hpp"
#include "Room.hpp"
#include "MeshRT.hpp"
#include "utils/utils.hpp"
#include "Sphere.hpp"
#include "Triangle.hpp"
#include "MeshInfo.hpp"

static UBO uboSpheres;
static UBO uboTriangles;
static UBO uboMeshesInfos;

static Sphere* spheresBuf = nullptr;
static Triangle* trianglesBuf = nullptr;
static MeshInfo* meshesInfosBuf = nullptr;

static void allocateSpheres() {
  GLsizeiptr size = sizeof(Sphere) * MAX_SPHERES;
  GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

  uboSpheres = UBO(1);
  uboSpheres.storage(size, flags);
  spheresBuf = (Sphere*)uboSpheres.map(size, flags);
}

static void allocateTriangles() {
  GLsizeiptr size = sizeof(Triangle) * MAX_TRIANGLES;
  GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

  uboTriangles = UBO(1);
  uboTriangles.storage(size, flags);
  trianglesBuf = (Triangle*)uboTriangles.map(size, flags);
}

static void allocateMeshes() {
  GLsizeiptr size = sizeof(MeshInfo) * MAX_MESHES;
  GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

  uboMeshesInfos = UBO(1);
  uboMeshesInfos.storage(size, flags);
  meshesInfosBuf = (MeshInfo*)uboMeshesInfos.map(size, flags);
}

namespace scene {

void scene1(RayTracingData& rtData) {
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

  if (!spheresBuf)
    allocateSpheres();

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

    updateSpheresBuffer(sphere, i);
    q = glm::angleAxis(PI * 0.03f, global::right);
    spawnFromBigSphereCenterDir = q * spawnFromBigSphereCenterDir;
  }
}

void scene2(RayTracingData& rtData) {
  rtData.numMeshes = 1;
  rtData.enableEnvLight = true;

  MeshRT rtMeshKnight;
  rtMeshKnight.loadOBJ("res/obj/Knight.obj", 0.05f);

  u32 totalNumTriangles = 0;
  for (int i = 0; i < 1; i++) {
    totalNumTriangles += rtMeshKnight.triangles.size();

    if (totalNumTriangles > MAX_TRIANGLES)
      error("[Scene::scene2] Mesh amount of triangles [{}] exceeds the limit [{}]", totalNumTriangles, MAX_TRIANGLES);
  }

  if (!trianglesBuf)   allocateTriangles();
  if (!meshesInfosBuf) allocateMeshes();

  // Add meshes to buffers
  u32 firstTriangleIndex = 0;
  updateMeshBuffer(firstTriangleIndex, &rtMeshKnight, 1);
}

void scene3(RayTracingData& rtData) {
  rtData.numMeshes = 1 + ROOM_TOTAL_MESHES; // Knight + room
  rtData.enableEnvLight = false;
  u32 totalNumTriangles = 0;

  // ===== Knight meshes ==================================== //

  MeshRT rtMeshKnight;
  rtMeshKnight.loadOBJ("res/obj/Knight.obj", 0.05f, {0.f, -15.f, 0.f});
  rtMeshKnight.rotate(PI_3, -global::up);

  totalNumTriangles += rtMeshKnight.triangles.size();

  // ===== Room meshes ====================================== //

  rtData.room = Room(vec3(0.f), 30.f, 30.f, 30.f);
  MeshRT* rtRoomMeshes = rtData.room.getMeshes();

  // ===== Preparing scene ================================== //

  totalNumTriangles += ROOM_TOTAL_TRIANGLES;

  if (totalNumTriangles > MAX_TRIANGLES)
    error("[Scene::scene3] Mesh amount of triangles [{}] exceeds the limit [{}]", totalNumTriangles, MAX_TRIANGLES);

  if (!trianglesBuf)   allocateTriangles();
  if (!meshesInfosBuf) allocateMeshes();

  // ===== Add meshes to buffers ============================ //

  u32 firstTriangleIndex = 0;
  updateMeshBuffer(firstTriangleIndex, rtRoomMeshes, ROOM_TOTAL_MESHES); // Room
  updateMeshBuffer(firstTriangleIndex, &rtMeshKnight, 1, ROOM_TOTAL_MESHES); // Knight
}

void scene4(RayTracingData& rtData) {
  rtData.numMeshes = ROOM_TOTAL_MESHES;
  rtData.numSpheres = 4;
  rtData.enableEnvLight = false;
  rtData.numRaysPerPixel = 5;
  rtData.numRayBounces = 5;

  // ===== Room meshes ====================================== //

  RayTracingMaterial floorMaterial;
  floorMaterial.color = vec4(1.f);
  floorMaterial.flags |= RT_MATERIAL_FLAG_CHECKERED_PATTERN;

  rtData.room = Room(vec3(0.f), 50.f, 20.f, 20.f);
  rtData.room.updateMaterial(ROOM_IDX_FLOOR, floorMaterial);
  rtData.room.updateMaterial(ROOM_IDX_RIGHT, {{global::green, 1.f}});
  rtData.room.updateMaterial(ROOM_IDX_FRONT, {{global::blue, 1.f}});
  MeshRT* rtRoomMeshes = rtData.room.getMeshes();

  // ===== Preparing scene ================================== //

  u32 totalNumTriangles = 0;
  totalNumTriangles += ROOM_TOTAL_TRIANGLES;

  if (totalNumTriangles > MAX_TRIANGLES)
    error("[Scene::scene4] Mesh amount of triangles [{}] exceeds the limit [{}]", totalNumTriangles, MAX_TRIANGLES);

  if (!spheresBuf)     allocateSpheres();
  if (!trianglesBuf)   allocateTriangles();
  if (!meshesInfosBuf) allocateMeshes();

  // ===== Spheres ========================================== //

  RayTracingMaterial material;
  material.color = vec4(1.f);
  material.emissionColor = vec3(0.f);
  material.emissionStrength = 0.f;

  float r = 4.f;
  vec3 initPos = vec3(0.f);
  vec3 offset = vec3(0.f);
  initPos.x -= r * rtData.numSpheres;

  for (int i = 0; i < rtData.numSpheres; i++) {
    material.smoothness = (float)i / (rtData.numSpheres - 1);

    Sphere sphere;
    sphere.pos = initPos + offset;
    sphere.radius = r;
    sphere.material = material;

    updateSpheresBuffer(sphere, i);
    offset.x += r * 2.f + 2.f;
  }

  // ===== Add room to buffers ============================== //

  u32 firstTriangleIndex = 0;
  updateMeshBuffer(firstTriangleIndex, rtRoomMeshes, ROOM_TOTAL_MESHES);
}

const Sphere& getSphere(size_t idx) {
  return spheresBuf[idx];
}

void updateSpheresBuffer(const Sphere& sphere, size_t idx) {
  if (!spheresBuf)
    error("[scene::updateSpheresBuffer] spheresBuf is not allocated");

  spheresBuf[idx] = sphere;
}

void updateMeshBuffer(u32& firstTriIdx, MeshRT* meshes, int numMeshes, int meshIdxOffset) {
  if (!meshesInfosBuf)
    error("[scene::updateMeshBuffer] meshesInfosBuf is not allocated");

  if (!trianglesBuf)
    error("[scene::updateMeshBuffer] trianglesBuf is not allocated");

  GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
  if (fence) {
    glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, UINT64_MAX);
    glDeleteSync(fence);
  }

  for (int i = 0; i < numMeshes; i++) {
    MeshRT& mesh = meshes[i];
    mesh.meshInfo.firstTriangleIndex = firstTriIdx;

    for (size_t j = 0; j < mesh.triangles.size(); j++) {
      trianglesBuf[j + firstTriIdx] = mesh.triangles[j];
    }

    meshesInfosBuf[i + meshIdxOffset] = mesh.meshInfo;

    firstTriIdx += mesh.triangles.size();
  }
}

void setUnifrom(const Shader& shader) {
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

void bind() {
  uboSpheres.bind();
  uboTriangles.bind();
  uboMeshesInfos.bind();
}

void unbind() {
  uboSpheres.unbind();
  uboTriangles.unbind();
  uboMeshesInfos.unbind();
}

} // namespace scenes

