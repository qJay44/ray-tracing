#version 460 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex;

out vec2 texCoord;

void main() {
  texCoord = in_tex;
	gl_Position = vec4(in_pos, 1.0f);
}

