#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec4 normal;
} gs_in[];

const float MAGNITUDE = 0.4;

layout(std140) uniform Matrices
{
	mat4 M;
	mat4 V;
	mat4 P;
};

void GenerateLine(int index)
{
    gl_Position = P * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = P * ((gl_in[index].gl_Position + gs_in[index].normal) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}