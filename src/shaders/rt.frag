#version 460 core

#define FLT_MAX 3.4028235e38f
#define PI 3.141592265359f

#define MAX_SPHERES 6u
#define MAX_TRIANGLES 500u
#define MAX_MESHES 10u

#define RT_MATERIAL_FLAG_CHECKERED_PATTERN 1u

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
  float smoothness;
  uint flags;
};

struct Sphere {
  vec3 pos;
  float r;
  RayTracingMaterial material;
};

struct Triangle {
  vec3 a;
  vec3 b;
  vec3 c;
  vec3 normalA;
  vec3 normalB;
  vec3 normalC;
};

struct MeshInfo {
  uint firstTriangleIndex;
  uint numTriangles;
  vec3 boundsMin;
  vec3 boundsMax;
  RayTracingMaterial material;
};

struct HitInfo {
  bool didHit;
  float dst;
  vec3 hitPoint;
  vec3 normal;
  RayTracingMaterial material;
};

const RayTracingMaterial rtMaterialInit = RayTracingMaterial(vec4(0.f), vec3(0.f), 0.f, 0.f, 0);
const HitInfo hitInfoInit = HitInfo(false, 0.f, vec3(0.f), vec3(0.f), rtMaterialInit);

uniform vec2 u_resolution;
uniform vec3 u_lightPos;
uniform vec3 u_groundColor;
uniform vec3 u_skyHorizonColor;
uniform vec3 u_skyZenithColor;
uniform vec3 u_camPos;
uniform mat4 u_camInv;
uniform sampler2D u_screenColorTexDefault;
uniform sampler2D u_screenDepthTex;
uniform int u_numRaysPerPixel;
uniform int u_numRenderedFrames;
uniform int u_numRayBounces;
uniform int u_numSpheres;
uniform int u_numMeshes;
uniform int u_enableEnvironmentalLight;
uniform float u_sunFocus;
uniform float u_sunIntensity;

layout(std140) uniform u_spheresBlock {
  Sphere spheres[MAX_SPHERES];
};

layout(std140) uniform u_trianglesBlock {
  Triangle triangles[MAX_TRIANGLES];
};

layout(std140) uniform u_meshesInfosBlock {
  MeshInfo meshesInfos[MAX_MESHES];
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
  hitInfo.dst = 0.f;
  vec3 offsetRayOrigin = ray.origin - sphereCenter;

  float a = dot(ray.dir, ray.dir);
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

HitInfo rayTriangle(Ray ray, Triangle tri) {
  vec3 ab = tri.b - tri.a;
  vec3 ac = tri.c - tri.a;
  vec3 triNormal = cross(ab, ac);
  vec3 ao = ray.origin - tri.a;
  vec3 dao = cross(ao, ray.dir);

  float determinant = -dot(ray.dir, triNormal);
  float invDet = 1.f / determinant;

  float dst = dot(ao, triNormal) * invDet;
  float u =  dot(ac, dao) * invDet;
  float v = -dot(ab, dao) * invDet;
  float w = 1.f - u - v;

  HitInfo hitInfo;
  hitInfo.didHit = determinant >= 1e-6f && dst >= 0.f && u >= 0.f && v >= 0.f && w >= 0.f;
  hitInfo.hitPoint = ray.origin + ray.dir * dst;
  hitInfo.normal = normalize(tri.normalA * w + tri.normalB * u + tri.normalC * v);
  hitInfo.dst = dst;

  return hitInfo;
}

bool rayInBoundingBox(Ray ray, vec3 boundsMin, vec3 boundsMax) {
  vec3 invDir = 1.f / ray.dir;
  vec3 tMin = (boundsMin - ray.origin) * invDir;
  vec3 tMax = (boundsMax - ray.origin) * invDir;
  vec3 t1 = min(tMin, tMax);
  vec3 t2 = max(tMin, tMax);
  float tNear = max(max(t1.x, t1.y), t1.z);
  float tFar  = min(min(t2.x, t2.y), t2.z);

  return tNear <= tFar;
}

HitInfo calcRayCollision(Ray ray) {
  HitInfo closestHit = hitInfoInit;
  closestHit.dst = FLT_MAX;

  for (int i = 0; i < u_numSpheres; i++) {
    Sphere sphere = spheres[i];
    HitInfo hitInfo = raySphere(ray, sphere.pos, sphere.r);

    if (hitInfo.didHit && hitInfo.dst < closestHit.dst) {
      closestHit = hitInfo;
      closestHit.material = sphere.material;
    }
  }

  for (int i = 0; i < u_numMeshes; i++) {
    MeshInfo meshInfo = meshesInfos[i];

    if (rayInBoundingBox(ray, meshInfo.boundsMin, meshInfo.boundsMax))
      for (int j = 0; j < meshInfo.numTriangles; j++) {
        Triangle tri = triangles[meshInfo.firstTriangleIndex + j];
        HitInfo hitInfo = rayTriangle(ray, tri);

        if (hitInfo.didHit && hitInfo.dst < closestHit.dst) {
          closestHit = hitInfo;
          closestHit.material = meshInfo.material;
        }
      }
  }

  return closestHit;
}

uint rngState = uint(gl_FragCoord.x + gl_FragCoord.y * u_resolution.x) + u_numRenderedFrames * 719393;

float randomValue() {
  rngState = rngState * 747796405 + 2891336453;
  uint result = ((rngState >> ((rngState >> 28) + 4)) ^ rngState) * 277803737;
  result = (result >> 22) ^ result;

  return result / 4294967295.f;
}

float randomValueNormalDistribution() {
  float theta = 2.f * PI * randomValue();
  float rho = sqrt(-2.f * log(randomValue()));
  return rho * cos(theta);
}

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

vec3 getEnvironmentLight(Ray ray) {
  vec3 lightDir = normalize(ray.origin - u_lightPos);
  float skyGradientT = pow(smoothstep(0.f, 0.4f, ray.dir.y), 0.35f);
  vec3 skyGradient = mix(u_skyHorizonColor, u_skyZenithColor, skyGradientT);
  float sun = pow(max(0, dot(ray.dir, -lightDir)), u_sunFocus) * u_sunIntensity;

  float groundToSkyT = smoothstep(-0.01f, 0.f, ray.dir.y);
  float sunMask = float(groundToSkyT >= 1.f);

  return mix(u_groundColor, skyGradient, groundToSkyT) + sun * sunMask;
}

vec3 trace(Ray ray) {
  vec3 incomingLight = vec3(0.f);
  vec3 rayColor = vec3(1.f);

  for (int i = 0; i < u_numRayBounces; i++) {
    HitInfo hitInfo = calcRayCollision(ray);
    if (hitInfo.didHit) {
      RayTracingMaterial material = hitInfo.material;
      if (material.flags & RT_MATERIAL_FLAG_CHECKERED_PATTERN) {
        vec2 c = mod(floor(hitInfo.hitPoint.xz * 0.35f), 2.f);
        material.color = c.x == c.y ? material.color : vec4(material.emissionColor, 1.f);
      }

      ray.origin = hitInfo.hitPoint;
      vec3 diffuseDir = normalize(hitInfo.normal + randomDirection());
      vec3 specularDir = reflect(ray.dir, hitInfo.normal);
      ray.dir = mix(diffuseDir, specularDir, hitInfo.material.smoothness);

      vec3 emittedLight = material.emissionColor * material.emissionStrength;
      incomingLight += emittedLight * rayColor;
      rayColor *= material.color.rgb;

    } else {
      if (u_enableEnvironmentalLight)
        incomingLight += getEnvironmentLight(ray) * rayColor;
      break;
    }
  }

  return incomingLight;
}

void main() {
  Ray ray = calcRay();
  vec3 color = texture(u_screenColorTexDefault, texCoord).rgb;

  vec3 totalIncomingLight = vec3(0.f);
  for (int i = 0; i < u_numRaysPerPixel; i++) {
    totalIncomingLight += trace(ray);
  }

  color += totalIncomingLight / u_numRaysPerPixel;

  FragColor = vec4(color, 1.f);
}

