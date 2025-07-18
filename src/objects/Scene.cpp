#include "Scene.hpp"

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

  if ((size_t)rtData.numSpheres > MAX_SPHERES)
    error("[Scene::scene1] The number of spheres [{}] exceeds the limit [{}]", rtData.numSpheres, MAX_SPHERES);

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
  rtData.numMeshes = 8;
  rtData.enableEnvLight = false;

  MeshRT* meshesRT = new MeshRT[rtData.numMeshes];

  // Knight
  meshesRT[0].loadOBJ("res/obj/Knight.obj", 0.08f, {0.f, -20.f, 0.f});
  meshesRT[0].rotate(PI_3, -global::up);

  // ==================== Walls ==================== //

  vec3 roomSize = vec3(20.f);
  vec3 roomCenter{0.f, 0.f, 0.f};
  vec2 wallSize = vec2{roomSize.x, roomSize.y} * 2.f;

  RayTracingMaterial wallRed;
  wallRed.color = {global::red, 1.f};

  RayTracingMaterial wallBlue;
  wallBlue.color = {global::blue, 1.f};

  RayTracingMaterial wallGray;
  wallGray.color = vec4(vec3(0.1f), 1.f);

  RayTracingMaterial wallBlack;
  wallBlack.color = vec4(vec3(0.f), 1.f);

  RayTracingMaterial wallWhite;
  wallWhite.color = vec4(1.f);

  RayTracingMaterial wallGreen;
  wallGreen.color = {global::green, 1.f};

  RayTracingMaterial wallLamp;
  wallLamp.color = wallWhite.color;
  wallLamp.emissionColor = wallLamp.color;
  wallLamp.emissionStrength = 15.f;

  meshesRT[1].createQuad({roomCenter.x - roomSize.x, roomCenter.y - roomSize.y, roomCenter.z + roomSize.z},  global::up,      -global::forward,  global::right  , wallSize, wallRed);   // Left wall
  meshesRT[2].createQuad({roomCenter.x + roomSize.x, roomCenter.y - roomSize.y, roomCenter.z - roomSize.z},  global::up,       global::forward, -global::right  , wallSize, wallBlue);  // Right wall
  meshesRT[3].createQuad({roomCenter.x - roomSize.x, roomCenter.y - roomSize.y, roomCenter.z - roomSize.z},  global::up,       global::right  ,  global::forward, wallSize, wallGray);  // Back wall
  meshesRT[4].createQuad({roomCenter.x + roomSize.x, roomCenter.y - roomSize.y, roomCenter.z + roomSize.z},  global::up,      -global::right  , -global::forward, wallSize, wallBlack); // Front wall
  meshesRT[5].createQuad({roomCenter.x - roomSize.x, roomCenter.y + roomSize.y, roomCenter.z + roomSize.z},  global::right,   -global::forward, -global::up     , wallSize, wallWhite); // Top wall (ceiling)
  meshesRT[6].createQuad({roomCenter.x - roomSize.x, roomCenter.y - roomSize.y, roomCenter.z + roomSize.z}, -global::forward,  global::right  ,  global::up     , wallSize, wallGreen); // Bottom wall (floor)

  // Lamp
  meshesRT[7].createQuad({roomCenter.x - roomSize.x * 0.5f, roomCenter.y + roomSize.y * 0.9f, roomCenter.z + roomSize.z * 0.5f}, global::right, -global::forward, -global::up, wallSize * 0.5f, wallLamp);

  // =============================================== //

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

