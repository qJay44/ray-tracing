#include "Camera.hpp"

#include <cassert>

#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glm/trigonometric.hpp"

#include "mesh/Mesh.hpp"
#include "mesh/meshes.hpp"

Camera::Camera(vec3 pos, vec3 orientation, double sensitivity)
  : position(pos),
    orientation(orientation),
    sensitivity(sensitivity) {
  farPlane = 100.f;
  calcView();
};

const float& Camera::getNearPlane()     const { return nearPlane;   }
const float& Camera::getFarPlane()      const { return farPlane;    }
const float& Camera::getSpeed()         const { return speed;       }
const float& Camera::getFov()           const { return fov;         }
const vec3&  Camera::getOrientation()   const { return orientation; }
const vec3&  Camera::getPosition()      const { return position;    }
const vec3&  Camera::getUp()            const { return up;          }
const mat4&  Camera::getMatrix()        const { return mat;         }
const mat4&  Camera::getMatrixInverse() const { return matInv;      }

vec3 Camera::getBack()    const { return  transpose(view)[2]; }
vec3 Camera::getLeft()    const { return -transpose(view)[0]; }
vec3 Camera::getRight()   const { return -getLeft(); }
vec3 Camera::getForward() const { return -getBack(); }
vec3 Camera::getDown()    const { return -getUp(); }

void Camera::setNearPlane(const float& p) { nearPlane = p; }
void Camera::setFarPlane(const float& p) { farPlane = p; }
void Camera::setSpeed(const float& s) { speed = s; }

void Camera::update(bool ignoreMousePos) {
  ivec2 winSize;
  glfwGetWindowSize(global::window, &winSize.x, &winSize.y);
  dvec2 winCenter = dvec2(winSize) / 2.;

  dvec2 mousePos;
  glfwGetCursorPos(global::window, &mousePos.x, &mousePos.y);

  if (ignoreMousePos)
    moveByMouse(winCenter);
  else
    moveByMouse(mousePos);

  calcView();

  float aspectRatio = static_cast<float>(winSize.x) / winSize.y;
  mat4 proj = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);

  mat = proj * view;
  matInv = glm::inverse(mat);
}

void Camera::draw(Camera& camToDraw, const Shader& shader, u32 flags) const {
  if (&camToDraw != this) {
    camToDraw.update(true);
    const vec3& camToDrawPos = camToDraw.position;

    Mesh camMesh = meshes::cube(camToDrawPos, 0.1f, {1.f, 0.f, 1.f});
    Mesh frustumMesh = meshes::frustum(camToDraw);

    if (flags & CAMERA_FLAG_DRAW_RIGHT)
      meshes::line(camToDrawPos, camToDrawPos + camToDraw.getRight(), {1.f, 0.f, 0.f}).draw(this, shader);

    if (flags & CAMERA_FLAG_DRAW_UP)
      meshes::line(camToDrawPos, camToDrawPos + camToDraw.up, {0.f, 1.f, 0.f}).draw(this, shader);

    if (flags & CAMERA_FLAG_DRAW_FORWARD)
      meshes::line(camToDrawPos, camToDrawPos + camToDraw.getForward(), {0.f, 0.f, 1.f}).draw(this, shader);

    if (flags & CAMERA_FLAG_DRAW_MESH)
      camMesh.draw(this, shader);

    if (flags & CAMERA_FLAG_DRAW_FRUSTUM)
      frustumMesh.draw(this, shader);

    if (flags & CAMERA_FLAG_DRAW_RAYS)
      meshes::cameraRays(camToDraw, {20, 20}).draw(this, shader);
  }
}

void Camera::moveForward() { position +=  orientation * speed * global::dt; }
void Camera::moveBack()    { position += -orientation * speed * global::dt; }
void Camera::moveLeft()    { position += -normalize(cross(orientation, up)) * speed * global::dt; }
void Camera::moveRight()   { position +=  normalize(cross(orientation, up)) * speed * global::dt; }

void Camera::moveUp()   { position +=  up * speed * global::dt; }
void Camera::moveDown() { position += -up * speed * global::dt; }

void Camera::moveByMouse(const dvec2& mousePos) {
  dvec2 winSize;
  {
    ivec2 winSizeTemp;
    glfwGetWindowSize(global::window, &winSizeTemp.x, &winSizeTemp.y);
    winSize = dvec2(winSizeTemp);
  }
  dvec2 winCenter = winSize / 2.;

  // Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
  // and then "transforms" them into degrees
  dvec2 rot = sensitivity * (mousePos - winCenter) / winSize;
  dvec2 radRot = glm::radians(-rot);

  calcOrientation(radRot.y, radRot.x);
}

void Camera::calcView() {
  view = lookAt(position, position + orientation, up);
}

void Camera::calcOrientation(const float& radRotX, const float& radRotY) {
  // Calculates upcoming vertical change in the Orientation
  vec3 newOrientation = rotate(orientation, radRotX, normalize(cross(orientation, up)));

  // Decides whether the next vertical Orientation is legal or not
  if (!(angle(newOrientation, up) <= glm::radians(5.f) || angle(newOrientation, -up) <= glm::radians(5.f)))
    orientation = newOrientation;

  // Rotates the Orientation left and right
  orientation = rotate(orientation, radRotY, up);
}

