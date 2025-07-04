#pragma once

#include "Shader.hpp"

#define CAMERA_FLAG_DRAW_RIGHT    1
#define CAMERA_FLAG_DRAW_UP       1 << 1
#define CAMERA_FLAG_DRAW_FORWARD  1 << 2
#define CAMERA_FLAG_DRAW_MESH     1 << 3
#define CAMERA_FLAG_DRAW_FRUSTUM  1 << 4
#define CAMERA_FLAG_DRAW_RAYS     1 << 5

#define CAMERA_FLAG_DRAW_DIRECTIONS (CAMERA_FLAG_DRAW_RIGHT | CAMERA_FLAG_DRAW_UP | CAMERA_FLAG_DRAW_FORWARD)

class Camera {
public:
  Camera() = delete;
  Camera(Camera&) = delete;
  Camera(Camera&&) = delete;
  Camera(vec3 pos, vec3 orientation, double sensitivity);

  const float& getNearPlane()     const;
  const float& getFarPlane()      const;
  const float& getSpeed()         const;
  const float& getFov()           const;
  const vec3&  getOrientation()   const;
  const vec3&  getPosition()      const;
  const vec3&  getUp()            const;
  const mat4&  getMatrix()        const;
  const mat4&  getMatrixInverse() const;

  vec3 getBack()    const;
  vec3 getLeft()    const;
  vec3 getRight()   const;
  vec3 getForward() const;
  vec3 getDown()  const;

  void setNearPlane(const float& p);
  void setFarPlane(const float& p);
  void setSpeed(const float& s);

  void update(bool ignoreMousePos = false);
  void draw(Camera& camToDraw, const Shader& shader, u32 flags = 0) const;

  virtual void moveForward();
  virtual void moveBack();
  virtual void moveLeft();
  virtual void moveRight();
  virtual void moveUp();
  virtual void moveDown();

protected:
  vec3 position, orientation;
  double sensitivity;

  float nearPlane = 0.1f;
  float farPlane = 100.f;
  float fov = 45.f;

  vec3 up{0.f, 1.f, 0.f};
  float speed = 5.f;

  mat4 view   = mat4(1.f);
  mat4 mat    = mat4(1.f);
  mat4 matInv = mat4(1.f);

protected:
  virtual void moveByMouse(const dvec2& mousePos);
  virtual void calcView();

  void calcOrientation(const float& radRotX, const float& radRotY);

private:
  friend struct gui;
  friend struct InputsHandler;
};

