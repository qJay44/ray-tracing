#pragma once

#include "MeshBase.hpp"

#include "EBO.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "vertex.hpp"

template<typename T>
class Mesh : public MeshBase {
public:
  Mesh() {};

  Mesh(const std::vector<Vertex4>&  vertices, const std::vector<GLuint>& indices, GLenum mode = GL_TRIANGLES, bool autoclear = true);
  Mesh(const std::vector<Vertex1>&  vertices, const std::vector<GLuint>& indices, GLenum mode = GL_TRIANGLES, bool autoclear = true);
  Mesh(const std::vector<VertexPT>& vertices, const std::vector<GLuint>& indices, GLenum mode = GL_TRIANGLES, bool autoclear = true);
  Mesh(const std::vector<VertexPC>& vertices, const std::vector<GLuint>& indices, GLenum mode = GL_TRIANGLES, bool autoclear = true);
  Mesh(const std::vector<Vertex1>& vertices, GLenum mode, bool autoclear = true);
  Mesh(const std::vector<Vertex4>& vertices, GLenum mode, bool autoclear = true);
  Mesh(const std::vector<VertexPC>& vertices, GLenum mode = GL_TRIANGLES, bool autoclear = true);
  Mesh(const std::vector<VertexPC>& vertices, const std::vector<mat4>& mats, GLenum mode = GL_TRIANGLES, bool autoclear = true);

  static Mesh<Vertex4> loadObj(const fspath& file, bool printInfo = false);

  ~Mesh() {
    if (autoclear)
      clear();
  }

  u32 getVerticesSize() const { return vertices.size(); }

  void draw(const Camera* camera, const Shader& shader, bool forceNoWireframe = false) const {
    vao.bind();

    mat4 model = translation * rotation * scaleMat;

    shader.setUniform1f("u_camNear", camera->getNearPlane());
    shader.setUniform1f("u_camFar", camera->getFarPlane());
    shader.setUniform1f("u_camFov", camera->getFov());
    shader.setUniform3f("u_camPos", camera->getPosition());
    shader.setUniform3f("u_camRight", camera->getRight());
    shader.setUniform3f("u_camUp", camera->getUp());
    shader.setUniform3f("u_camForward", camera->getForward());
    shader.setUniformMatrix4f("u_cam", camera->getMatrix());
    shader.setUniformMatrix4f("u_camInv", camera->getMatrixInverse());
    shader.setUniformMatrix4f("u_model", model);

    if (global::drawWireframe & !forceNoWireframe)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (instancingCount) {
      if (indices.size())
        glDrawElementsInstanced(mode, indices.size(), GL_UNSIGNED_INT, 0, instancingCount);
      else
        glDrawArraysInstanced(mode, 0, vertices.size(), instancingCount);
    } else {
      if (indices.size())
        glDrawElements(mode, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
      else
        glDrawArrays(mode, 0, (GLsizei)vertices.size());
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    vao.unbind();
  }

  void clear() {
    if (vao.size) vao.clear();
    if (vbo.size) vbo.clear();
    if (ebo.size) ebo.clear();
    if (instancingVBO.size) instancingVBO.clear();
  }

private:
  std::vector<T> vertices;

  VAO vao;
  VBO vbo;
  EBO ebo;

  VBO instancingVBO;
  size_t instancingCount = 0;
};

