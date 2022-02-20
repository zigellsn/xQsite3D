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
    vec3 direction;
};

#define NR_POINT_LIGHTS 4
uniform Light light[NR_POINT_LIGHTS];

out vec4 out_color;

in PASS {
    vec4 normal;
    vec4 position;
    vec2 tex_coord;
    vec4 light_space;
} pass;

uniform sampler2D shadowSampler;

uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace, Light singleLight)
{
    float bias = max(0.05 * (1.0 - dot(pass.normal, vec4(singleLight.direction, 0.0))), 0.005);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowSampler, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    return shadow;
}

vec4 CalcLight(Light singleLight)
{
    vec3 tmpAmbient = vec3(1.0);
    if (material.useDiffuseMap == false) {
        tmpAmbient = singleLight.ambient * material.ambient;
    } else {
        tmpAmbient = singleLight.ambient * vec3(texture(material.diffuseMap, pass.tex_coord));
    }
    vec4 ambient = vec4(tmpAmbient, 1.0);


    vec4 norm = normalize(pass.normal);
    vec4 lightDir = normalize(vec4(singleLight.position, 1.0) - pass.position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 tmpDiffuse = vec3(1.0);
    if (material.useDiffuseMap == false) {
        tmpDiffuse = singleLight.diffuse * (diff * material.diffuse);
    } else {
        tmpDiffuse = singleLight.diffuse * diff * vec3(texture(material.diffuseMap, pass.tex_coord));
    }
    vec4 diffuse = vec4(tmpDiffuse, 1.0);

    vec3 viewDir = normalize(viewPos - pass.position.xyz);
    vec3 reflectDir = reflect(-lightDir.xyz, norm.xyz);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 tmpSpecular = vec3(1.0);
    if (material.useDiffuseMap == false) {
        tmpSpecular = singleLight.specular * (spec * material.specular);
    } else {
        tmpSpecular = singleLight.specular * spec * vec3(texture(material.specularMap, pass.tex_coord));
    }
    vec4 specular = vec4(tmpSpecular, 1.0);

    float shadow = ShadowCalculation(pass.light_space, singleLight);

    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

void main(){

    vec4 output_color = vec4(0.0);
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        output_color += CalcLight(light[i]);

    out_color = output_color;
}
