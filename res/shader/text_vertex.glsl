#version 330 core

uniform vec2 pos;

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec2 texture;

out vec4 pass_color;
out vec2 pass_tex_coord;

void main(){
    gl_Position = vec4(pos.x / 2.0 + position.x, pos.y / 2.0 + position.y, 0.0, 1.0);
	pass_tex_coord = texture;
}
