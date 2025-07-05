#version 460 core

#define MAX_SPHERES 10u
#define FLT_MAX 3.4028235e38f

out vec4 FragColor;

in vec2 texCoord;

struct Ray {
  vec3 origin;
  vec3 dir;
};

struct RayTracingMaterial {
  vec4 color;
};

struct Sphere {
  vec3 pos;
  float r;
  RayTracingMaterial material;
};

struct HitInfo {
  bool didHit;
  float dst;
  vec3 hitPoint;
  vec3 normal;
  RayTracingMaterial material;
};

const HitInfo hitInfoInit = HitInfo(false, FLT_MAX, vec3(0.f), vec3(0.f), RayTracingMaterial(vec4(0.f)));

uniform vec2 u_resolution;
uniform vec3 u_camPos;
uniform mat4 u_camInv;
uniform sampler2D u_screenColorTex;
uniform sampler2D u_screenDepthTex;

layout(std140) uniform u_spheresBlock {
  Sphere spheres[MAX_SPHERES];
};

vec3 calcViewVec() {
  vec2 ndc = texCoord * 2.f - 1.f;
  vec4 clipPos = vec4(ndc, -1.f, 1.f);
  vec4 worldPos = u_camInv * clipPos;
  worldPos /= worldPos.w;

  return worldPos.xyz - u_camPos;
}

Ray calcRay() {
  Ray r;
  r.origin = u_camPos;
  r.dir = normalize(calcViewVec());

  return r;
}

HitInfo raySphere(Ray ray, vec3 sphereCenter, float sphereRadius) {
  HitInfo hitInfo = hitInfoInit;
  vec3 offsetRayOrigin = ray.origin - sphereCenter;

  float a = 1.f;
  float b = 2.f * dot(offsetRayOrigin, ray.dir);
  float c = dot(offsetRayOrigin, offsetRayOrigin) - sphereRadius * sphereRadius;
  float discriminant = b * b - 4.f * a * c;

  if (discriminant >= 0.f) {
    float dst = (-b - sqrt(discriminant)) / (2.f * a);

    if (dst >= 0.f) {
      hitInfo.didHit = true;
      hitInfo.dst = dst;
      hitInfo.hitPoint = ray.origin + ray.dir * dst;
      hitInfo.normal = normalize(hitInfo.hitPoint - sphereCenter);
    }
  }

  return hitInfo;
}

HitInfo calcRayCollision(Ray ray) {
  HitInfo closestHit = hitInfoInit;

  for (int i = 0; i < MAX_SPHERES; i++) {
    Sphere sphere = spheres[i];
    HitInfo hitInfo = raySphere(ray, sphere.pos, sphere.r);

    if (hitInfo.didHit && hitInfo.dst < closestHit.dst) {
      closestHit = hitInfo;
      closestHit.material = sphere.material;
    }
  }

  return closestHit;
}

void main() {
  Ray ray = calcRay();
  vec4 color = texture(u_screenColorTex, texCoord);
  HitInfo closestHit = calcRayCollision(ray);

  if (closestHit.didHit)
    color += closestHit.material.color;

  FragColor = color;
}

