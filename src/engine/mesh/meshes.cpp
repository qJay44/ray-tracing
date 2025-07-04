#include "meshes.hpp"

#include <cmath>
#include <vector>

#include "../Camera.hpp"
#include "../frustum/Frustum.hpp"

namespace meshes {

Mesh<VertexPC> line(vec3 p1, vec3 p2, vec3 color, bool clearable) {
  std::vector<VertexPC> vertices{
    {p1, color},
    {p2, color}
  };

  return Mesh<VertexPC>(vertices, GL_LINES, clearable);
}

Mesh<Vertex4> axis(float size, bool clearable) {
  std::vector<Vertex4> vertices{
    {{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}},
    {{size, 0.f, 0.f}, {1.f, 0.f, 0.f}},
    {{0.f, 0.f, 0.f}, {0.f, 1.f, 0.f}},
    {{0.f, size, 0.f}, {0.f, 1.f, 0.f}},
    {{0.f, 0.f, 0.f}, {0.f, 0.f, 1.f}},
    {{0.f, 0.f, size}, {0.f, 0.f, 1.f}},
  };

  return Mesh<Vertex4>(vertices, GL_LINES, clearable);
}

Mesh<Vertex4> plane(vec3 pos, vec2 size, vec3 color, bool clearable) {
  std::vector<Vertex4> vertices{
    {{-0.1f, -0.1f, 0.f}, color, {0.f, 0.f}},
    {{-0.1f,  0.1f, 0.f}, color, {0.f, 1.f}},
    {{ 0.1f,  0.1f, 0.f}, color, {1.f, 1.f}},
    {{ 0.1f, -0.1f, 0.f}, color, {1.f, 0.f}},
  };

  std::vector<GLuint> indices{
    0, 1, 2,
    2, 3, 0
  };


  Mesh m = Mesh<Vertex4>(vertices, indices, GL_TRIANGLES, clearable);
  m.translate(pos);
  m.scale(size);

  return m;
}

Mesh<Vertex1> plane1(vec3 pos, vec2 size, bool clearable) {
  std::vector<Vertex1> vertices{
    {{-0.1f, -0.1f, 0.f}},
    {{-0.1f,  0.1f, 0.f}},
    {{ 0.1f,  0.1f, 0.f}},
    {{ 0.1f, -0.1f, 0.f}},
  };

  std::vector<GLuint> indices{
    0, 1, 2,
    2, 3, 0
  };


  Mesh m = Mesh<Vertex1>(vertices, indices, GL_TRIANGLES, clearable);
  m.translate(pos);
  m.scale(size);

  return m;
}


Mesh<Vertex4> cube(vec3 pos, float size, vec3 color, bool clearable) {
  //        5--------6
  //       /|       /|
  //      1--------2 |
  //      | |      | |
  //      | 4------|-7
  //      |/       |/
  //      0--------3
  //
  //      x: - to left, + to right
  //      y: - to down, + to up
  //      z: - to rear, + to front

  vec2 tex{0.f, 0.f};
  std::vector<Vertex4> vertices{
    {{-1.f, -1.f,  1.f}, color, tex, {-1.f, -1.f,  1.f}},
    {{-1.f,  1.f,  1.f}, color, tex, {-1.f,  1.f,  1.f}},
    {{ 1.f,  1.f,  1.f}, color, tex, { 1.f,  1.f,  1.f}},
    {{ 1.f, -1.f,  1.f}, color, tex, { 1.f, -1.f,  1.f}},
    {{-1.f, -1.f, -1.f}, color, tex, {-1.f, -1.f, -1.f}},
    {{-1.f,  1.f, -1.f}, color, tex, {-1.f,  1.f, -1.f}},
    {{ 1.f,  1.f, -1.f}, color, tex, { 1.f,  1.f, -1.f}},
    {{ 1.f, -1.f, -1.f}, color, tex, { 1.f, -1.f, -1.f}},
  };

  std::vector<GLuint> indices{
    0, 1, 2, //  Front face
    2, 3, 0, //
    4, 5, 6, //  Rear face
    6, 7, 4, //
    0, 1, 5, //  Left face
    5, 4, 0, //
    3, 2, 6, //  Right face
    6, 7, 3, //
    1, 5, 6, //  Top face
    6, 2, 1, //
    0, 4, 7, //  Bottom face
    7, 3, 0, //
  };

  Mesh m = Mesh<Vertex4>(vertices, indices, GL_TRIANGLES, clearable);
  m.translate(pos);
  m.scale(size);

  return m;
}

Mesh<VertexPC> frustum(const Camera& cam, vec3 color, bool clearable) {
  frustum::Frustum frustum(cam);

  ivec2 winSize;
  glfwGetWindowSize(global::window, &winSize.x, &winSize.y);
  float aspect = static_cast<float>(winSize.x) / winSize.y;

  vec3 camCrossUp = normalize(cross(cam.getForward(), cam.getLeft()));
  float fovRad = glm::radians(cam.getFov());
  float fovSize = 0.f;
  if (fovRad >= PI_2) {
    fovSize = 57.28f;
    fovRad -= PI_2;
  }
  fovSize += tan(fovRad);

  // NOTE: Frustum's forward is pointing towards +z

  float farVSideHalf = cam.getFarPlane() * fovSize * 0.5f;
  float farHSideHalf = farVSideHalf * aspect;
  vec3 frontMultFar = cam.getForward() * cam.getFarPlane();
  vec3 farPos = cam.getPosition() + frontMultFar;
  vec3 farTR = farPos + cam.getLeft()  * farHSideHalf +  camCrossUp * farVSideHalf;
  vec3 farTL = farPos + cam.getRight() * farHSideHalf +  camCrossUp * farVSideHalf;
  vec3 farBL = farPos + cam.getRight() * farHSideHalf + -camCrossUp * farVSideHalf;
  vec3 farBR = farPos + cam.getLeft()  * farHSideHalf + -camCrossUp * farVSideHalf;

  float nearVSideHalf = cam.getNearPlane() * fovSize * 0.5f;
  float nearHSideHalf = nearVSideHalf * aspect;
  vec3 nearPos = cam.getPosition() + cam.getForward() * cam.getNearPlane();
  vec3 nearTR = nearPos + cam.getLeft()  * nearHSideHalf +  camCrossUp * nearVSideHalf;
  vec3 nearTL = nearPos + cam.getRight() * nearHSideHalf +  camCrossUp * nearVSideHalf;
  vec3 nearBL = nearPos + cam.getRight() * nearHSideHalf + -camCrossUp * nearVSideHalf;
  vec3 nearBR = nearPos + cam.getLeft()  * nearHSideHalf + -camCrossUp * nearVSideHalf;

  vec3 centerTR = (farTR + nearTR) * 0.5f;
  vec3 centerTL = (farTL + nearTL) * 0.5f;
  vec3 centerBR = (farBR + nearBR) * 0.5f;
  vec3 centerBL = (farBL + nearBL) * 0.5f;

  vec3 centerTop    = (centerTR + centerTL) * 0.5f;
  vec3 centerBottom = (centerBR + centerBL) * 0.5f;
  vec3 centerRight  = (centerTR + centerBR) * 0.5f;
  vec3 centerLeft   = (centerTL + centerBL) * 0.5f;

  std::vector<VertexPC> vertices {
    // Near plane
    {nearTR, color},
    {nearTL, color},
    {nearBL, color},
    {nearBR, color},
    // Far plane
    {farTR, color},
    {farTL, color},
    {farBL, color},
    {farBR, color},
    // Normals
    {centerTop, global::green},
    {centerTop + frustum.topFace.normal, global::green},
    {centerBottom, global::green},
    {centerBottom + frustum.bottomFace.normal, global::green},
    {centerRight, global::red},
    {centerRight + frustum.rightFace.normal, global::red},
    {centerLeft, global::red},
    {centerLeft + frustum.leftFace.normal, global::red},
    {nearPos, global::blue},
    {nearPos + frustum.nearFace.normal, global::blue},
    {farPos, global::blue},
    {farPos + frustum.farFace.normal, global::blue},
  };

  std::vector<GLuint> indices {
    // Near plane
    0, 1,
    1, 2,
    2, 3,
    3, 0,
    // Far plane
    4, 5,
    5, 6,
    6, 7,
    7, 4,
    // Connect corners
    0, 4,
    1, 5,
    2, 6,
    3, 7,
    // Normals
    8, 9,
    10, 11,
    12, 13,
    14, 15,
    16, 17,
    18, 19
  };

  return Mesh<VertexPC>(vertices, indices, GL_LINES, clearable);
}

Mesh<VertexPC> cameraRays(const Camera& cam, ivec2 count, vec3 color, bool clearable) {
  ivec2 winSize;
  glfwGetWindowSize(global::window, &winSize.x, &winSize.y);
  float aspect = static_cast<float>(winSize.x) / winSize.y;

  float planeHeight = cam.getNearPlane() * tan(glm::radians(cam.getFov() * 0.5f)) * 2.f;
  float planeWidth = planeHeight * aspect;
  vec3 bottomLeftLocal = vec3(-planeWidth * 0.5f, -planeHeight * 0.5f, cam.getNearPlane());

  const vec3& camPos = cam.getPosition();
  std::vector<VertexPC> vertices;
  vec3 camCrossUp = normalize(cross(cam.getForward(), cam.getRight()));

  for (int i = 0; i < count.x; i++) {
    for (int j = 0; j < count.y; j++) {
      float tx = i / (count.x - 1.f);
      float ty = j / (count.y - 1.f);

      vec3 pointLocal = bottomLeftLocal + vec3(planeWidth * tx, planeHeight * ty, 0.f);
      vec3 point = camPos +
        cam.getRight()   * pointLocal.x +
        camCrossUp       * pointLocal.y +
        cam.getForward() * pointLocal.z;

      vertices.push_back({point, color});
    }
  }

  return Mesh<VertexPC>(vertices, GL_POINTS, clearable);
}

Mesh<VertexPT> screen(bool clearable) {
  std::vector<VertexPT> vertices{
    {{-1.f, -1.f, 0.f}, {0.f, 0.f}},
    {{-1.f,  1.f, 0.f}, {0.f, 1.f}},
    {{ 1.f,  1.f, 0.f}, {1.f, 1.f}},
    {{ 1.f, -1.f, 0.f}, {1.f, 0.f}},
  };

  std::vector<GLuint> indices{
    0, 1, 2,
    2, 3, 0
  };

  return Mesh<VertexPT>(vertices, indices, GL_TRIANGLES, clearable);
}

} // namespace meshes

