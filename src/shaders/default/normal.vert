#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTex;
layout(location = 3) in vec3 inNormal;

out DATA {
  vec3 color;
  vec2 tex;
  vec3 normal;
} data_out;

uniform mat4 u_model;

void main() {
  data_out.color = inColor;
  data_out.tex = inTex;
  data_out.normal = inNormal;
  gl_Position = u_model * vec4(inPos, 1.0f);
}
