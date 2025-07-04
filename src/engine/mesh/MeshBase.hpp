#pragma once

#include <vector>

#include "glm/gtc/quaternion.hpp"

#include "../Camera.hpp"
#include "../Shader.hpp"

#define MESH_VERTEX_ATTRIBUTES 11 // 3 (position) + 3 (color) + 2 (texture) + 3 (normal)
#define MESH_TEXTURE_LIMIT 10

#define MESH_FLAG_DRAW_RIGHT    1
#define MESH_FLAG_DRAW_UP       1 << 2
#define MESH_FLAG_DRAW_FORWARD  1 << 3

#define MESH_FLAG_DRAW_DIRECTIONS (AIRPLANE_FLAG_DRAW_RIGHT | AIRPLANE_FLAG_DRAW_UP | AIRPLANE_FLAG_DRAW_FORWARD)

class MeshBase {
public:
  MeshBase(const std::vector<GLuint>& indices, GLenum mode, bool autoclear);

  const mat4& getTranslation() const;
  const mat4& getRotation()    const;
  const mat4& getScale()       const;
  mat4 getModel() const;
  u32 getIndicesSize() const;

  void translate(const vec3& v);
  void rotate(const float& angle, const vec3& axis);
  void rotate(const glm::quat& q);
  void scale(const float& s);
  void scale(const vec2& s);

  void drawAxis(const Camera* camera, const Shader& shader) const;

protected:
  MeshBase();

protected:
  std::vector<GLuint> indices;
  GLenum mode;

  mat4 translation = mat4(1.f);
  mat4 rotation    = mat4(1.f);
  mat4 scaleMat    = mat4(1.f);

  bool autoclear;
};

