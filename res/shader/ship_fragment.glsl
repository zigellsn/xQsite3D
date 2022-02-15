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

#define NR_POINT_LIGHTS 4
uniform Light light[NR_POINT_LIGHTS];

out vec4 out_color;

in vec4 pass_normal;
in vec4 pass_frag_pos;
in vec2 pass_tex_coord;

uniform vec3 viewPos;

vec4 CalcLight(Light singleLight)
{
    vec3 tmpAmbient = vec3(1.0);
    if (material.useDiffuseMap == false) {
        tmpAmbient = singleLight.ambient * material.ambient;
    } else {
        tmpAmbient = singleLight.ambient * vec3(texture(material.diffuseMap, pass_tex_coord));
    }
    vec4 ambient = vec4(tmpAmbient, 1.0);


    vec4 norm = normalize(pass_normal);
    vec4 lightDir = normalize(vec4(singleLight.position, 1.0) - pass_frag_pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 tmpDiffuse = vec3(1.0);
    if (material.useDiffuseMap == false) {
        tmpDiffuse = singleLight.diffuse * (diff * material.diffuse);
    } else {
        tmpDiffuse = singleLight.diffuse * diff * vec3(texture(material.diffuseMap, pass_tex_coord));
    }
    vec4 diffuse = vec4(tmpDiffuse, 1.0);

    vec3 viewDir = normalize(viewPos - pass_frag_pos.xyz);
    vec3 reflectDir = reflect(-lightDir.xyz, norm.xyz);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 tmpSpecular = vec3(1.0);
    if (material.useDiffuseMap == false) {
        tmpSpecular = singleLight.specular * (spec * material.specular);
    } else {
        tmpSpecular = singleLight.specular * spec * vec3(texture(material.specularMap, pass_tex_coord));
    }
    vec4 specular = vec4(tmpSpecular, 1.0);

    return ambient + diffuse + specular;
}


void main(){

    vec4 output_color = vec4(0.0);
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        output_color += CalcLight(light[i]);

    out_color = output_color;
}
