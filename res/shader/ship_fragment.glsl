#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;

    sampler2D diffuseMap;
    bool useDiffuseMap;

    sampler2D specularMap;
    bool useSpecularMap;
};

uniform Material material;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

out vec4 out_color;

in vec4 pass_normal;
in vec4 pass_frag_pos;
in vec2 pass_tex_coord;

uniform vec3 viewPos;

void main(){
    vec3 tmpAmbient = vec3(1.0);
    if (material.useDiffuseMap == false) {
        tmpAmbient = light.ambient * material.ambient;
    } else {
        tmpAmbient = light.ambient * vec3(texture(material.diffuseMap, pass_tex_coord));
    }
    vec4 ambient = vec4(tmpAmbient, 1.0);


    vec4 norm = normalize(pass_normal);
    vec4 lightDir = normalize(vec4(light.position, 1.0) - pass_frag_pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 tmpDiffuse = vec3(1.0);
    if (material.useDiffuseMap == false) {
        tmpDiffuse = light.diffuse * (diff * material.diffuse);
    } else {
        tmpDiffuse = light.diffuse * diff * vec3(texture(material.diffuseMap, pass_tex_coord));
    }
    vec4 diffuse = vec4(tmpDiffuse, 1.0);

    vec3 viewDir = normalize(viewPos - pass_frag_pos.xyz);
    vec3 reflectDir = reflect(-lightDir.xyz, norm.xyz);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 tmpSpecular = vec3(1.0);
    if (material.useDiffuseMap == false) {
        tmpSpecular = light.specular * (spec * material.specular);
    } else {
        tmpSpecular = light.specular * spec * vec3(texture(material.specularMap, pass_tex_coord));
    }
    vec4 specular = vec4(tmpSpecular, 1.0);

    out_color = ambient + diffuse + specular;
}
