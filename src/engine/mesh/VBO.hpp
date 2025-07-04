#pragma once

// Vertex Buffer Object
struct VBO {
  GLuint id;
  GLsizei size = 0;

  VBO() {}

  VBO(GLsizei size, const void* data, GLsizeiptr dataSize) : size(size) {
    glGenBuffers(size, &id);
    bind();
    glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
  }

  static void unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

  void bind()   const { glBindBuffer(GL_ARRAY_BUFFER, id); }
  void clear()        { glDeleteBuffers(size, &id); size = 0; }
};

