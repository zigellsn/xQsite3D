#version 330 core

layout (location = 0) in vec4 position;

out vec3 pass_tex_coord;

layout(std140) uniform Matrices
{
	mat4 M;
	mat4 V;
	mat4 P;
	mat4 N;
};

void main()
{
    pass_tex_coord = position.xyz;
    vec4 pos = P * V * position;
    gl_Position = pos.xyww;
}