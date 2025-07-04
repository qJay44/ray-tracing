#pragma once

#include "mesh/texture/Texture.hpp"

struct FBO {
  GLuint id;
  GLsizei size = 0;

  FBO() {}

  FBO(GLsizei size) : size(size) {
    glGenFramebuffers(size, &id);
  }

  static void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

  void bind()   const { glBindFramebuffer(GL_FRAMEBUFFER, id); }
  void clear()        { glDeleteFramebuffers(size, &id); size = 0; }

  void attach2D(GLenum attachment, const Texture& tex) const {
    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex.id, 0);
    unbind();
  }
};

