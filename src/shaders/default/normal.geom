#version 460 core

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in DATA {
  vec3 color;
  vec2 tex;
  vec3 normal;
} data_in[];

uniform mat4 u_cam;

const float scale = 0.1f;

void main() {
  gl_Position = u_cam * gl_in[0].gl_Position;
  EmitVertex();
  gl_Position = u_cam * (gl_in[0].gl_Position + vec4(data_in[0].normal, 0.f) * scale);
  EmitVertex();
  EndPrimitive();

  gl_Position = u_cam * gl_in[1].gl_Position;
  EmitVertex();
  gl_Position = u_cam * (gl_in[1].gl_Position + vec4(data_in[1].normal, 0.f) * scale);
  EmitVertex();
  EndPrimitive();

  gl_Position = u_cam * gl_in[2].gl_Position;
  EmitVertex();
  gl_Position = u_cam * (gl_in[2].gl_Position + vec4(data_in[2].normal, 0.f) * scale);
  EmitVertex();
  EndPrimitive();
}
