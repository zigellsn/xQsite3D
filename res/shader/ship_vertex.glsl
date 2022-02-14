#version 330 core

layout(std140) uniform Matrices
{
	mat4 M;
	mat4 V;
	mat4 P;
};

layout (location = 0) in vec4 position;
// layout (location = 1) in vec4 color;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec2 texture;

out vec4 pass_color;
out vec4 pass_normal;
out vec4 pass_frag_pos;
out vec2 pass_tex_coord;

void main(){
    gl_Position = P * V * M * position;
    pass_color = normal;
    // calc N-matrix in app
    pass_normal = mat4(transpose(inverse(M))) * normal;;
    pass_tex_coord = texture;
    pass_frag_pos = M * position;
}
 