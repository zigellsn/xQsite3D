#version 330 core

layout(std140) uniform Matrices
{
	mat4 MVP;
	mat4 M;
	mat4 V;
	mat4 P;
	mat4 N;
};

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

out vec4 pass_color;

void main(void) {
	gl_Position = P * V * M * position;
	pass_color = color;
}