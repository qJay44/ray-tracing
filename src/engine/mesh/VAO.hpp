#pragma once

// Vertex Array Object
struct VAO {
  GLuint id;
  GLsizei size = 0;

  VAO() {}

  VAO(GLsizei size) : size(size) {
    glGenVertexArrays(size, &id);
  }

  static void unbind() { glBindVertexArray(0); }

  void bind()   const { glBindVertexArray(id); }
  void clear()        { glDeleteVertexArrays(size, &id); size = 0; }

  void linkAttrib(GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, void* offset) const {
    glEnableVertexAttribArray(layout);
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
  }
};

