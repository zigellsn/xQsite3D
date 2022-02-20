#version 330 core

layout (location = 0) in vec4 position;

layout(std140) uniform Matrices
{
	mat4 M;
	mat4 V;
	mat4 P;
	mat4 N;
	mat4 L;
};

void main()
{
    gl_Position = L * M * position;
}