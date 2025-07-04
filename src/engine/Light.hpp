#pragma once

#include "../engine/mesh/Mesh.hpp"

class Light : public Mesh<Vertex4> {
public:
  Light(vec3 position, float size = 1.f, vec3 color = {1.f, 1.f, 1.f});

  const vec3& getPosition() const;
  const vec3& getColor() const;
  const vec3& getWaveLengths() const;

private:
  friend struct gui;

  vec3 position;
  vec3 color;
  vec3 waveLengths{700.f, 530.f, 440.f};
};

