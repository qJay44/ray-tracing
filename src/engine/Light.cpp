#include "Light.hpp"

#include "../engine/mesh/meshes.hpp"

Light::Light(vec3 position, float size, vec3 color) : Mesh(meshes::cube(position, size, color, false)), position(position), color(color) {}

const vec3& Light::getPosition() const { return position; }
const vec3& Light::getColor() const { return color; }
const vec3& Light::getWaveLengths() const { return waveLengths; }

