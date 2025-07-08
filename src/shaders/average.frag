#version 460 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D u_screenColorTexOld;
uniform sampler2D u_screenColorTexNew;
uniform int u_numRenderedFrames;

void main() {
  vec4 colorOld = texture(u_screenColorTexOld, texCoord);
  vec4 colorNew = texture(u_screenColorTexNew, texCoord);

  float weight = 1.f / u_numRenderedFrames;
  vec4 accumulatedAverage = colorOld * (1.f - weight) + colorNew * weight;

  FragColor = accumulatedAverage;
}

