#pragma once

struct RBO {
  GLuint id;
  GLsizei size = 0;

  RBO() {}

  RBO(GLsizei size) : size(size) {
    glGenRenderbuffers(size, &id);
  }

  static void unbind() { glBindRenderbuffer(GL_RENDERBUFFER, 0); }

  void bind()   const { glBindRenderbuffer(GL_RENDERBUFFER, id); }
  void clear()        { glDeleteRenderbuffers(size, &id); size = 0; }

  void storage(GLenum internalFormat, ivec2 size) const {
    bind();
    glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, size.x, size.y);
    unbind();
  }
};

