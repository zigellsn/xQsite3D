#version 330 core

layout(std140) uniform Matrices
{
	mat4 M;
	mat4 V;
	mat4 P;
	mat4 N;
	mat4 L;
};

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec2 texture;

out PASS {
    vec4 normal;
    vec4 position;
    vec2 tex_coord;
    vec4 light_space;
} pass;

void main(){
    gl_Position = P * V * M * position;
    pass.normal = V * N * normal;
    pass.tex_coord = texture;
    pass.position = M * position;
    pass.light_space = L * pass.position, 1.0;
}
 