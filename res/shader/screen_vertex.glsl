#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texture;

out vec2 pass_tex_coord;

void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    pass_tex_coord = texture;
}