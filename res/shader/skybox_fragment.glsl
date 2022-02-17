#version 330 core
out vec4 FragColor;

in vec3 pass_tex_coord;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, pass_tex_coord);
}