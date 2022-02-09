#include "objects/Light.h"
#include "ShaderProgram.h"

Light::Light(glm::vec4 position) : GLObject() {
    modelMatrix = glm::mat4();
    modelMatrix = glm::translate(modelMatrix, glm::vec3(position));
}

void Light::draw(glm::mat4 view) {
//    if (shader != nullptr) {
//        shader->begin();

        glm::vec4 pos = view * modelMatrix[3];

//        cwc::block lights;
//        cwc::blockValue light = {&pos[0], 4 * sizeof(float)};
//        lights.insert(cwc::blockPair("Position_worldspace", light));
//
//        light = {&ambient[0], 4 * sizeof(float)};
//        lights.insert(cwc::blockPair("LAmbient", light));
//
//        light = {&diffuse[0], 4 * sizeof(float)};
//        lights.insert(cwc::blockPair("LDiffuse", light));
//
//        light = {&specular[0], 4 * sizeof(float)};
//        lights.insert(cwc::blockPair("LSpecular", light));

//        shader->setUniformBlock("Lights", lights, 2);

//        shader->end();
//    }
}

void Light::setSpecular(glm::vec4 color) {
    this->specular = color;
}

glm::vec4 Light::getSpecular() {
    return specular;
}

void Light::setAmbient(glm::vec4 color) {
    this->ambient = color;
}

glm::vec4 Light::getAmbient() {
    return ambient;
}

void Light::setDiffuse(glm::vec4 color) {
    this->diffuse = color;
}

glm::vec4 Light::getDiffuse() {
    return diffuse;
}

void Light::drawElements() {

}

Light::~Light() = default;