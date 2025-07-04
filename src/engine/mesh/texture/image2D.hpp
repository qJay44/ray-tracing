#pragma once

struct image2D {
  static void write(const std::string& path, uvec2 size, u8 channels, byte* buf);

  image2D();
  image2D(const fspath& path, bool flipVertically = false);
  ~image2D();

  u16 width, height, channels;
  void* pixels = nullptr;
  std::string name;

  void load(const fspath& path, bool flipVertically = false);
  void loadTifInt16Single(const fspath& path, bool flipVertically = false);

private:
  bool stbiLoad = false;
  bool tifInt16Load = false;
};
