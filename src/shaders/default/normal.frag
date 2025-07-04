#version 460 core

in vec3 color;

out vec4 FragColor;

void main() {
	FragColor = vec4(vec3(1.f, 0.5f, 0.f), 1.f);
}
