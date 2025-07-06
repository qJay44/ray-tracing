#version 460 core

#define FLT_MAX 3.4028235e38f
#define PI 3.141592265359f

#define MAX_SPHERES 5u
#define MAX_BOUNCES 5u

out vec4 FragColor;

in vec2 texCoord;

struct Ray {
  vec3 origin;
  vec3 dir;
};

struct RayTracingMaterial {
  vec4 color;
  vec3 emissionColor;
  float emissionStrength;
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

const RayTracingMaterial rtMaterialInit = RayTracingMaterial(vec4(0.f), vec3(0.f), 0.f);
const HitInfo hitInfoInit = HitInfo(false, FLT_MAX, vec3(0.f), vec3(0.f), rtMaterialInit);

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

uint rngState = uint(gl_FragCoord.x + gl_FragCoord.y * u_resolution.x);

// float randomValue() {
//   rngState *= (rngState + 195439u) * (rngState + 124395u) * (rngState + 845921u);
//   return rngState / 4294967295.f;
// }

float randomValue() {
  rngState = rngState * 747796405 + 2891336453;
  uint result = ((rngState >> ((rngState >> 28) + 4)) ^ rngState) * 277803737;
  result = (result >> 22) ^ result;

  return result / 4294967295.f;
}

float randomValueNormalDistribution() {
  float theta = 2.f * PI * randomValue();
  float rho = sqrt(-2 * log(randomValue()));
  return rho * cos(theta);
}

// vec3 randomDirection() {
//   return normalize(vec3(
//     randomValue() * 2.f - 1.f,
//     randomValue() * 2.f - 1.f,
//     randomValue() * 2.f - 1.f
//   ));
// }

vec3 randomDirection() {
  return normalize(vec3(
    randomValueNormalDistribution(),
    randomValueNormalDistribution(),
    randomValueNormalDistribution()
  ));
}

vec3 randomHemisphereDirection(vec3 normal) {
  vec3 dir = randomDirection();
  return dir * sign(dot(normal, dir));
}

vec3 trace(Ray ray) {
  vec3 incomingLight = vec3(0.f);
  vec3 rayColor = vec3(1.f);

  for (int i = 0; i < MAX_BOUNCES; i++) {
    HitInfo hitInfo = calcRayCollision(ray);
    if (hitInfo.didHit) {
      ray.origin = hitInfo.hitPoint;
      ray.dir = randomHemisphereDirection(hitInfo.normal);

      RayTracingMaterial material = hitInfo.material;
      vec3 emittedLight = material.emissionColor * material.emissionStrength;
      incomingLight += emittedLight * rayColor;
      rayColor *= material.color.xyz;
    } else {
      break;
    }
  }

  return incomingLight;
}

void main() {
  Ray ray = calcRay();
  vec3 color = texture(u_screenColorTex, texCoord).rgb;

  color += trace(ray);

  FragColor = vec4(color, 1.f);
}

