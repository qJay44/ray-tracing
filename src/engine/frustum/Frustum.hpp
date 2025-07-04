#pragma once

#include "../Camera.hpp"
#include "Plane.hpp"

namespace frustum {

struct Frustum {
  Plane topFace;
  Plane bottomFace;

  Plane rightFace;
  Plane leftFace;

  Plane farFace;
  Plane nearFace;

  Frustum(const Camera& cam) {
    ivec2 winSize;
    glfwGetWindowSize(global::window, &winSize.x, &winSize.y);
    float aspectRatio = static_cast<float>(winSize.x) / winSize.y;

    const float& camNear = cam.getNearPlane();
    const float& camFar = cam.getFarPlane();
    const vec3& camPos = cam.getPosition();
    vec3 camCrossUp = normalize(cross(cam.getForward(), cam.getLeft()));

    float halfVSide = camFar * tanf(cam.getFov() * 0.5f);
    float halfHSide = halfVSide * aspectRatio;
    vec3 frontMultFar = camFar * cam.getForward();

    // Pointing inside the frustum
    vec3 right = normalize(cross(frontMultFar + cam.getLeft()  * halfHSide,  camCrossUp));
    vec3 left  = normalize(cross(frontMultFar + cam.getRight() * halfHSide, -camCrossUp));
    vec3 up    = normalize(cross(frontMultFar + camCrossUp     * halfVSide,  cam.getRight()));
    vec3 down  = normalize(cross(frontMultFar - camCrossUp     * halfVSide,  cam.getLeft()));

    farFace    = {cam.getBack(), dot(cam.getBack(), camPos + frontMultFar)};
    nearFace   = {cam.getForward(), dot(cam.getForward(), camPos + camNear * cam.getForward())};
    rightFace  = {right, dot(right, camPos)};
    leftFace   = {left , dot(left, camPos)};
    topFace    = {up   , dot(up, camPos)};
    bottomFace = {down , dot(down, camPos)};
  }
};

}

