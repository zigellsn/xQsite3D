#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

out vec4 out_color;

in vec4 pass_color;
in vec4 pass_normal;
in vec4 pass_frag_pos;
in vec2 pass_tex_coord;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D textureSampler;

void main(){
    vec3 tmpAmbient = lightColor * material.ambient;
    vec4 ambient = vec4(tmpAmbient, 1.0);

    vec4 norm = normalize(pass_normal);
    vec4 lightDir = normalize(vec4(lightPos, 1.0) - pass_frag_pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 tmpDiffuse = lightColor * (diff * material.diffuse);
    vec4 diffuse = vec4(tmpDiffuse, 1.0);

    vec3 viewDir = normalize(viewPos - pass_frag_pos.xyz);
    vec3 reflectDir = reflect(-lightDir.xyz, norm.xyz);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 tmpSpecular = lightColor * (spec * material.specular);
    vec4 specular = vec4(tmpSpecular, 1.0);

    out_color = (ambient + diffuse + specular) * texture(textureSampler, pass_tex_coord);
}
