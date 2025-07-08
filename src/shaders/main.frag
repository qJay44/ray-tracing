#version 460 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D u_screenColorTexFinal;

void main() {
  FragColor = texture(u_screenColorTexFinal, texCoord);
}

