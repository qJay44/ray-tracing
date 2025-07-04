#pragma once

#include "Mesh.hpp"

namespace meshes {

Mesh<VertexPC> line(vec3 p1, vec3 p2, vec3 color, bool clearable = true);
Mesh<Vertex4> axis(float size = 1.f, bool clearable = true);
Mesh<Vertex4> plane(vec3 position, vec2 size, vec3 color = {1.f, 1.f, 1.f}, bool clearable = true);
Mesh<Vertex1> plane1(vec3 position, vec2 size, bool clearable = true);
Mesh<Vertex4> cube(vec3 position, float size = 1.f, vec3 color = {1.f, 1.f, 1.f}, bool clearable = true);
Mesh<VertexPC> frustum(const Camera& cam, vec3 color = {1.f, 1.f, 1.f}, bool clearable = true);
Mesh<VertexPC> cameraRays(const Camera& cam, ivec2 count, vec3 color = {1.f, 1.f, 1.f}, bool clearable = true);
Mesh<VertexPT> screen(bool clearable = true);

} // namespace meshes

