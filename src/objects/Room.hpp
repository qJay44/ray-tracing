#pragma once

#include "MeshRT.hpp"
#include "RayTracingMaterial.hpp"

#define ROOM_IDX_LEFT    0u
#define ROOM_IDX_RIGHT   1u
#define ROOM_IDX_BACK    2u
#define ROOM_IDX_FRONT   3u
#define ROOM_IDX_CEILING 4u
#define ROOM_IDX_FLOOR   5u
#define ROOM_IDX_LAMP    6u

#define ROOM_TOTAL_MESHES 7u
#define ROOM_TOTAL_TRIANGLES (ROOM_TOTAL_MESHES * 2u)

class Room {
public:
  Room();
  Room(vec3 center, float width, float height, float depth, const vec3& lampPosScale = vec3(0.5f, 0.9f, 0.5f), float lampScale = 0.5f);

  MeshRT* getMeshes();
  const RayTracingMaterial& getWallMaterial(size_t idx) const;

  void updateMaterial(size_t idx, const RayTracingMaterial& material);

private:
  friend struct gui;

  MeshRT meshesRT[7];
};

