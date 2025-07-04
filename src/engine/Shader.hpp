#pragma once

#include "mesh/texture/Texture.hpp"
#include <string>

#define SHADER_DEFAULT_TYPE_COLOR_SHADER   1
#define SHADER_DEFAULT_TYPE_NORMALS_SHADER 1 << 1
#define SHADER_DEFAULT_TYPE_TEXTURE_SHADER 1 << 2

class Shader {
public:
  Shader();
  Shader(const fspath& vsPath, const fspath& fsPath, const fspath& gsPath = "");
  Shader(const fspath& compPath);

  static const Shader& getDefaultShader(u32 type);

  static void setDirectoryLocation(const fspath& path);
  static void setDefaultShader(u32 type, const fspath& vsPath, const fspath& fsPath, const fspath& gsPath = "");

  GLint getUniformLoc(const std::string& name) const;
  GLint getUniformBlockIndex(const std::string& name) const;

  void use() const;
  void clear();

  void setUniform1f(const GLint& loc, const GLfloat& n)    const;
  void setUniform3f(const GLint& loc, const vec3& v)       const;
  void setUniform4f(const GLint& loc, const vec4& v)       const;
  void setUniform1i(const GLint& loc, const GLint& v)      const;
  void setUniform1ui(const GLint& loc, const GLuint& v)    const;
  void setUniform2i(const GLint& loc, const ivec2& v)      const;
  void setUniform2f(const GLint& loc, const vec2& v)       const;
  void setUniformMatrix4f(const GLint& loc, const mat4& m) const;

  void setUniform1f(const std::string& name, const GLfloat& n)    const;
  void setUniform3f(const std::string& name, const vec3& v)       const;
  void setUniform4f(const std::string& name, const vec4& v)       const;
  void setUniform1i(const std::string& name, const GLint& v)      const;
  void setUniform1ui(const std::string& name, const GLuint& v)    const;
  void setUniform2i(const std::string& name, const ivec2& v)      const;
  void setUniform2f(const std::string& name, const vec2& v)       const;
  void setUniformMatrix4f(const std::string& name, const mat4& m) const;

  void setUniformTexture(const GLint& loc, const Texture& texture) const;
  void setUniformTexture(const Texture& texture) const;

  void setUniformBlock(const GLuint& idx, const GLuint& i) const;
  void setUniformBlock(const std::string& name, const GLuint& i) const;

  GLuint program = 0;
private:
  static fspath directory;
  static Shader defaultColor;
  static Shader defaultNormals;
  static Shader defaultTexture;


private:
  static GLuint load(fspath path, int type);
  static GLuint compile(const fspath& path, int type);
  static void link(GLuint program);
};

