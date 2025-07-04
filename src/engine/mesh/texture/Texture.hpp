#pragma once

#include "TexParams.hpp"

class Texture {
public:
  Texture();

  Texture(
    const ivec2& size,
    const GLint& internalFormat,
    const GLenum& format,
    const std::string& uniform,
    const GLuint& unit = 0,
    const GLenum& target = GL_TEXTURE_2D,
    const TexParams& params = TexParams{}
  );

  Texture(
    const fspath& path,
    const std::string& uniform,
    const GLuint& unit = 0,
    const GLenum& target = GL_TEXTURE_2D,
    const GLint& internalFormat = 0,
    const GLenum& format = 0,
    const GLenum& type = GL_UNSIGNED_BYTE,
    const TexParams& params = TexParams{}
  );

  Texture(
    const fspath& path0,                   // First part
    const fspath& path1,                   // Second part
    const std::string& uniform,            // Uniform name in shader
    const GLuint& unit,                    // Texture slot
    const GLenum& target,                  // Texture type
    const GLint& internalFormat,           // Color format in the OpenGL program
    const GLenum& format,                  // Color format of the given image(s)
    const GLenum& type,                    // Color bytes format of the given image(s)
    const TexParams& params = TexParams{}  // Texture parameters (glTexParameteri)
  );

  Texture(const Texture& other);

  void operator=(const Texture& other);

  void bind() const;
  void unbind() const;
  void clear();

  const GLenum& getTarget() const;
  const GLuint& getUnit() const;
  const std::string& getUniformName() const;
  const uvec3& getSize() const;

  void setUnit(GLuint unit);
  void setUniformName(const std::string& name);

private:
  GLenum target;
  std::string uniformName;
  GLuint unit;

  GLuint id;
  uvec3 size;

private:
  friend struct FBO;
};

