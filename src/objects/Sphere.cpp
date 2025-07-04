#include "Sphere.hpp"
#include "RayTracingMaterial.hpp"
#include <cstring>

Sphere::Sphere(vec3 position, float radius, RayTracingMaterial material)
  : position(position), radius(radius), material(material) {}

Sphere::UniformBlock Sphere::getUniformBlock() const {
  UniformBlock ub;
  ub.pos = position;
  ub.radius = radius;
  ub.materialColor = material.color;

  return ub;
}

