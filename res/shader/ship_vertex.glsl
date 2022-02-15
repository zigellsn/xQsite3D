#version 330 core

layout(std140) uniform Matrices
{
	mat4 M;
	mat4 V;
	mat4 P;
	mat4 N;
};

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec2 texture;

out vec4 pass_normal;
out vec4 pass_frag_pos;
out vec2 pass_tex_coord;

void main(){
    gl_Position = P * V * M * position;
    pass_normal = V * N * normal;
    pass_tex_coord = texture;
    pass_frag_pos = M * position;
}
 