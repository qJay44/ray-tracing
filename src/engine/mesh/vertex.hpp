#pragma once

struct Vertex1 {
  vec3 position;
};

struct Vertex4 {
  vec3 position;
  vec3 color;
  vec2 texture;
  vec3 normal;
};

struct VertexPT {
  vec3 position;
  vec2 texture;
};

struct VertexPC {
  vec3 position;
  vec3 color;
};

typedef Vertex1 VertexP;
typedef Vertex4 VertexPCTN;
