#version 330 core

out vec4 out_color;

in vec2 pass_tex_coord;

uniform sampler2D textureSampler;

void main()
{
    vec3 col = texture(textureSampler, pass_tex_coord).rgb;
    out_color = vec4(col, 1.0);
}