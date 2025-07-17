#pragma once

// Uniform Buffer Object
struct UBO {
  GLuint id = 0;
  GLsizei size = 0;

  UBO() {}

  UBO(GLsizei size) : size(size) {
    glGenBuffers(size, &id);
  }

  UBO(GLsizei size, const void* data, GLsizeiptr dataSize) : size(size) {
    glGenBuffers(size, &id);
    bind();
    glBufferData(GL_UNIFORM_BUFFER, dataSize, data, GL_STATIC_DRAW);
  }

  static void unbind() { glBindBuffer(GL_UNIFORM_BUFFER, 0); }

  void storage(GLsizeiptr size, GLbitfield flags) {
    bind();
    glBufferStorage(GL_UNIFORM_BUFFER, size, nullptr, flags);
    unbind();
  }

  [[nodiscard]]
  void* map(GLsizeiptr size, GLbitfield flags) {
    bind();
    void* ptr = glMapBufferRange(GL_UNIFORM_BUFFER, 0, size, flags);
    unbind();
    return ptr;
  }

  void bind()               const { glBindBuffer(GL_UNIFORM_BUFFER, id); }
  void bindBase(GLuint idx) const { glBindBufferBase(GL_UNIFORM_BUFFER, idx, id); }
  void clear()                    { glDeleteBuffers(size, &id); size = 0; }
};

