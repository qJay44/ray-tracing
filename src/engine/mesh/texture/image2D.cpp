#include "image2D.hpp"

#include <format>

#include "tiffio.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "utils/status.hpp"

void image2D::write(const std::string& path, uvec2 size, u8 channels, byte* buf) {
  stbi_flip_vertically_on_write(true);
  stbi_write_png(path.c_str(), size.x, size.y, channels, buf, size.x * channels);
}

image2D::image2D() {}

image2D::image2D(const fspath& path, bool flipVertically) { load(path, flipVertically); }

image2D::~image2D() {
  if (stbiLoad)
    stbi_image_free(pixels);
  else if(tifInt16Load)
    delete[] (s16*)pixels;
  else if (pixels)
    error("image2D is not nullptr");
}

void image2D::load(const fspath& path, bool flipVertically) {
  status::start("Loading", path.string());

  if (path.extension() == ".tif")
    loadTifInt16Single(path, flipVertically);
  else {
    int w, h, colorChannels;
    stbi_set_flip_vertically_on_load(flipVertically);
    pixels = stbi_load(path.string().c_str(), &w, &h, &colorChannels, 0);
    if (!pixels) {
      status::end(false);
      error(std::format("stb can't load the image: {}", path.string()));
    }

    width = static_cast<u16>(w);
    height = static_cast<u16>(h);
    channels = static_cast<u16>(colorChannels);
    name = path.string();
    stbiLoad = true;
  }

  status::end(true);
}

void image2D::loadTifInt16Single(const fspath& path, bool flipVertically) {
  TIFF* tif = TIFFOpen(path.string().c_str(), "r");
  if (!tif) {
    status::end(false);
    error(std::format("tif can't load the image: {}", path.string()));
  }

  u32 w, h;
  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
  s16* buf = new s16[w * h];

  for (u32 row = 0; row < h; row++) {
    s16* bufRow = buf + row * w;
    if (TIFFReadScanline(tif, bufRow, row) < 0)
      error("tif scanline read error");
  }

  if (flipVertically) {
    for (u32 y = 0; y < h / 2; ++y) {
      short* rowTop = buf + y * w;
      short* rowBottom = buf + (h - y - 1) * w;
      std::swap_ranges(rowTop, rowTop + w, rowBottom);
    }
  }

  width = static_cast<u16>(w);
  height = static_cast<u16>(h);
  channels = 1u;
  name = path.string();
  pixels = (void*)buf;
  tifInt16Load = true;

  TIFFClose(tif);
}

