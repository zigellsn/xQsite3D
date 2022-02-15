#include "objects/Light.h"
#include "ShaderProgram.h"

Light::Light(glm::vec4 position) : GLObject() {
    modelMatrix = glm::mat4();
    modelMatrix = glm::translate(modelMatrix, glm::vec3(position));
}

void Light::setSpecular(glm::vec4 color) {
    this->specular = color;
}

void Light::setAmbient(glm::vec4 color) {
    this->ambient = color;
}

void Light::setDiffuse(glm::vec4 color) {
    this->diffuse = color;
}

void Light::drawElements() {

}

Light::~Light() = default;