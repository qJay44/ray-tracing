#include "scenes.hpp"

#include "glm/gtc/quaternion.hpp"

namespace scenes {

void scene1(Sphere* spheresBuf) {
  constexpr vec4 palette[MAX_SPHERES] = {
    vec4(1.f, 0.f, 1.f, 1.f),
    vec4(0.f),
    vec4(0.f, 1.f, 0.f, 1.f),
    vec4(0.f, 0.f, 1.f, 1.f),
    vec4(1.f, 0.f, 0.f, 1.f),
    vec4(1.f)
  };

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

  for (size_t i = 1; i < MAX_SPHERES; i++) {
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

} // namespace scenes
