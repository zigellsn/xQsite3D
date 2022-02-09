#version 330 core

out vec4 out_color;

in vec4 pass_color;
in vec2 pass_tex_coord;

uniform sampler2D textureSampler;

void main(){
    out_color = texture(textureSampler, pass_tex_coord);
}