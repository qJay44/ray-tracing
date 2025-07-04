#include "Texture.hpp"

#include <cassert>
#include <format>
#include <windows.h>

#include "image2D.hpp"

constexpr GLenum GL_STANDARD_CHANNELS[4] = {GL_RED, GL_RG, GL_RGB, GL_RGBA};

Texture::Texture()
  : target(GL_TEXTURE_2D),
    uniformName(),
    unit(),
    id(0) {}

Texture::Texture(
  const ivec2& size,
  const GLint& internalFormat,
  const GLenum& format,
  const std::string& uniform,
  const GLuint& unit,
  const GLenum& target,
  const TexParams& params
)
  : target(target),
    uniformName(uniform),
    unit(unit)
{
  switch (target) {
    case GL_TEXTURE_2D: {
      glGenTextures(1, &id);
      bind();
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, params.minFilter);
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, params.magFilter);
      glTexParameteri(target, GL_TEXTURE_WRAP_S, params.wrapS);
      glTexParameteri(target, GL_TEXTURE_WRAP_T, params.wrapT);
      glTexImage2D(target, 0, internalFormat, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, NULL);
      break;
    }
    default:
      error("[Texture] Unhandled texture creation type: [{}]", target);
      break;
  }
  unbind();
}

Texture::Texture(
  const fspath& path,
  const std::string& uniform,
  const GLuint& unit,
  const GLenum& target,
  const GLint& internalFormat,
  const GLenum& format,
  const GLenum& type,
  const TexParams& params
)
  : target(target),
    uniformName(uniform),
    unit(unit)
{
  switch (target) {
    case GL_TEXTURE_2D: {
      image2D img(path, true);

      GLenum finalInternalFormat = internalFormat ? internalFormat : GL_STANDARD_CHANNELS[img.channels - 1];
      GLint finalFormat = format ? format : GL_STANDARD_CHANNELS[img.channels - 1];
      size = {img.width, img.height, 1};

      glGenTextures(1, &id);
      bind();
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, params.minFilter);
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, params.magFilter);
      glTexParameteri(target, GL_TEXTURE_WRAP_S, params.wrapS);
      glTexParameteri(target, GL_TEXTURE_WRAP_T, params.wrapT);
      glTexImage2D(target, 0, finalInternalFormat, img.width, img.height, 0, finalFormat, type, img.pixels);

      break;
    }
    case GL_TEXTURE_CUBE_MAP: {
      // NOTE: Order matters
      constexpr const char* texNames[6] = {
        "right",
        "left",
        "top",
        "bottom",
        "front",
        "back",
      };

      glGenTextures(1, &id);
      bind();
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, params.minFilter);
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, params.magFilter);
      glTexParameteri(target, GL_TEXTURE_WRAP_S, params.wrapS);
      glTexParameteri(target, GL_TEXTURE_WRAP_T, params.wrapT);
      glTexParameteri(target, GL_TEXTURE_WRAP_R, params.wrapR);

      // ========== Find out the extension of the images ==========  //

      // NOTE: Assuming all files have the same extention (per folder)

      char imgPath[256];
      sprintf(imgPath, "%s/*.*", path.string().c_str());

      WIN32_FIND_DATA fdFile;
      HANDLE hFind = NULL;
      if((hFind = FindFirstFile(imgPath, &fdFile)) == INVALID_HANDLE_VALUE)
        error("[Texture][GL_TEXTURE_CUBE_MAP] Path not found: [{}]\n", path.string());

      //Find first file will always return "."
      //    and ".." as the first two directories.
      FindNextFile(hFind, &fdFile);
      FindNextFile(hFind, &fdFile);
      if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        warning("[Texture][GL_TEXTURE_CUBE_MAP] Found directory in the cubemap folder: [{}]\n", fdFile.cFileName);

      FindNextFile(hFind, &fdFile);
      sprintf(imgPath, "%s/%s", path.string().c_str(), fdFile.cFileName);

      std::string extension = fspath(fdFile.cFileName).extension().string();
      FindClose(hFind);

      // ==========================================================  //

      for (int i = 0; i < 6; i++) {
        fspath filePath = std::format("{}/{}{}", path.string().c_str(), texNames[i], extension.c_str());
        image2D img(filePath);

        GLint finalInternalFormat = internalFormat ? internalFormat : GL_STANDARD_CHANNELS[img.channels - 1];
        GLenum finalFormat = format ? format : GL_STANDARD_CHANNELS[img.channels - 1];

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, finalInternalFormat, img.width, img.height, 0, finalFormat, type, img.pixels);
      }
      break;
    }
    default:
      error("[Texture] Unhandled texture creation type: [{}]", target);
      break;
  }
  unbind();
}

Texture::Texture(
  const fspath& path0,
  const fspath& path1,
  const std::string& uniform,
  const GLuint& unit,
  const GLenum& target,
  const GLint& internalFormat,
  const GLenum& format,
  const GLenum& type,
  const TexParams& params
)
  : target(target),
    uniformName(uniform),
    unit(unit)
{
  switch (target) {
    case GL_TEXTURE_2D_ARRAY: {
      image2D img0(path0, true);
      image2D img1(path1, true);

      assert(img0.width == img1.width);
      assert(img0.height == img1.height);
      assert(img0.channels == img1.channels);

      size = {img0.width, img0.height, 2};
      glGenTextures(1, &id);
      bind();
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, params.minFilter);
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, params.magFilter);
      glTexParameteri(target, GL_TEXTURE_WRAP_S, params.wrapS);
      glTexParameteri(target, GL_TEXTURE_WRAP_T, params.wrapT);
      glTexStorage3D(target, 1, internalFormat, size.x, size.y, 2);
      glTexSubImage3D(target, 0, 0, 0, 0, size.x, size.y, 1, format, type, img0.pixels);
      glTexSubImage3D(target, 0, 0, 0, 1, size.x, size.y, 1, format, type, img1.pixels);

      break;
    }
    default:
      error("[Texture][GL_TEXTURE_2D_ARRAY] Unhandled texture creation type: [{}]", target);
      break;
  }
  unbind();
}

Texture::Texture(const Texture& other) :
  target(other.target),
  uniformName(other.uniformName),
  unit(other.unit),
  id(other.id),
  size(other.size) {}

void Texture::operator=(const Texture& other) {
  target = other.target;
  uniformName = other.uniformName;
  unit = other.unit;
  id = other.id;
  size = other.size;
}

void Texture::bind() const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(target, id);
}

void Texture::unbind() const {
  glBindTexture(target, 0); //
}

void Texture::clear() {
  glDeleteTextures(1, &id); //
}

const GLenum& Texture::getTarget() const { return target; }
const GLuint& Texture::getUnit() const { return unit; }
const std::string& Texture::getUniformName() const { return uniformName; }
const uvec3& Texture::getSize() const { return size; }

void Texture::setUnit(GLuint unit) {
  this->unit = unit;
}

void Texture::setUniformName(const std::string& name) {
  uniformName = name;
}

