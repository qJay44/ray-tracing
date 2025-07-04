#pragma once

struct TexParams {
  GLenum minFilter = GL_LINEAR;
  GLenum magFilter = GL_LINEAR;
  GLenum wrapS = GL_REPEAT;
  GLenum wrapT = GL_REPEAT;
  GLenum wrapR = GL_REPEAT;
};

