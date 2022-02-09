#version 330 core

layout(std140) uniform Matrices
{
	mat4 MVP;
	mat4 M;
	mat4 V;
	mat4 P;
	mat4 N;
};

layout (location = 0) in vec4 position;
// layout (location = 1) in vec4 color;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec2 texture;

out vec4 pass_color;
out vec2 pass_tex_coord;

void main(){
    pass_color = normal;
    pass_tex_coord = texture;
    gl_Position = P * V * M * position;
}
 