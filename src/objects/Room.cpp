#include "Room.hpp"

Room::Room(vec3 center, float width, float height, float depth, const vec3& lampPosScale, float lampScale) {
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
  wallLamp.emissionStrength = 35.f;

  vec3 size{width, height, depth};
  vec3 sizeHalf = size * 0.5f;

  meshesRT[ROOM_IDX_LEFT]   .createQuad({center.x - sizeHalf.x, center.y - sizeHalf.y, center.z + sizeHalf.z},  global::up,      -global::forward,  global::right  , {depth, height}, wallRed);   // Left wall
  meshesRT[ROOM_IDX_RIGHT]  .createQuad({center.x + sizeHalf.x, center.y - sizeHalf.y, center.z - sizeHalf.z},  global::up,       global::forward, -global::right  , {depth, height}, wallBlue);  // Right wall
  meshesRT[ROOM_IDX_BACK]   .createQuad({center.x - sizeHalf.x, center.y - sizeHalf.y, center.z - sizeHalf.z},  global::up,       global::right  ,  global::forward, {width, height}, wallGray);  // Back wall
  meshesRT[ROOM_IDX_FRONT]  .createQuad({center.x + sizeHalf.x, center.y - sizeHalf.y, center.z + sizeHalf.z},  global::up,      -global::right  , -global::forward, {width, height}, wallBlack); // Front wall
  meshesRT[ROOM_IDX_CEILING].createQuad({center.x + sizeHalf.x, center.y + sizeHalf.y, center.z + sizeHalf.z}, -global::forward, -global::right  , -global::up     , {width, depth} , wallWhite); // Top wall (ceiling)
  meshesRT[ROOM_IDX_FLOOR]  .createQuad({center.x - sizeHalf.x, center.y - sizeHalf.y, center.z + sizeHalf.z}, -global::forward,  global::right  ,  global::up     , {width, depth} , wallGreen); // Bottom wall (floor)

  vec3 lampOffset = sizeHalf * lampPosScale;
  meshesRT[ROOM_IDX_LAMP].createQuad(center + lampOffset, -global::forward, -global::right, -global::up, vec2{width, depth} * lampScale, wallLamp);
}

const MeshRT* Room::getMeshes() const {
  return meshesRT;
}

void Room::update(size_t idx, const RayTracingMaterial& material) {
  meshesRT[idx].meshInfo.material = material;
}

