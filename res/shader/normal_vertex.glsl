#version 330 core

layout(std140) uniform Matrices
{
	mat4 M;
	mat4 V;
	mat4 P;
};

out VS_OUT {
    vec4 normal;
} vs_out;

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;

void main(){
    gl_Position = V * M * position;
    mat4 normalMatrix = transpose(inverse(V * M));
    vs_out.normal = normalize(normalMatrix * vec4(normal.xyz, 1.0));
}
