#include "MeshBase.hpp"

#include "glm/ext/matrix_transform.hpp"
#include "meshes.hpp"

MeshBase::MeshBase(const std::vector<GLuint>& indices, GLenum mode, bool autoclear)
  : indices(indices),
    mode(mode),
    autoclear(autoclear)
{
  this->indices.resize(indices.size());
  this->indices.reserve(indices.size());
}

MeshBase::MeshBase() : autoclear(false) {}

const mat4& MeshBase::getTranslation() const { return translation; }
const mat4& MeshBase::getRotation()    const { return rotation;    }
const mat4& MeshBase::getScale()       const { return scaleMat;    }

mat4 MeshBase::getModel() const {
  return translation * rotation * scaleMat;
}

u32 MeshBase::getIndicesSize()  const { return indices.size();  }

void MeshBase::translate(const vec3& v) { translation = glm::translate(translation, v); }

void MeshBase::rotate(const float& angle, const vec3& axis) { rotation = glm::rotate(rotation, angle, axis);}
void MeshBase::rotate(const glm::quat& q) { rotation = glm::mat4_cast(q) * rotation; };

void MeshBase::scale(const float& s)    { scaleMat = glm::scale(scaleMat, vec3(s)); }
void MeshBase::scale(const vec2& s)     { scaleMat = glm::scale(scaleMat, vec3(s, 1.f)); }

void MeshBase::drawAxis(const Camera* camera, const Shader& shader) const {
  static Mesh axis = meshes::axis(3.f);
  axis.translation = translation;
  axis.rotation = rotation;
  axis.scaleMat = scaleMat;
  axis.draw(camera, shader);
}

