#include "objects/GLObject.h"

GLObject::GLObject(const std::string &name) : modelMatrix(glm::mat4(1.0f)), normalMatrix(glm::mat4(1.0f)) {
    this->name = name;
}

void GLObject::translate(glm::vec3 moveDirection, float speed) {
    glm::vec3 right = modelMatrix[0];
    glm::mat3 newMatrix = glm::mat4(1.0f);
    if (mode == Mode::FLY) {
        newMatrix = glm::mat3(-right, up, direction);
    } else {
        newMatrix = glm::mat3({1.0f, 0.0f, 0.0f},
                              {0.0f, 1.0f, 0.0f},
                              {direction.x, 0.0f, direction.z});
    }
    glm::vec3 nDirection = glm::normalize(moveDirection);
    nDirection = newMatrix * moveDirection;
    position += nDirection * speed;
    updateModelMatrix();
}

glm::vec4 GLObject::getPosition() {
    glm::vec4 ret = this->modelMatrix[3];
    return ret;
}

void GLObject::roll(float angle, float speed) {
    up = glm::normalize(glm::rotate(up, angle * speed, direction));
    updateModelMatrix();
}

void GLObject::pitch(float angle, float speed) {
    if (flipX)
        angle = -angle;
    direction = glm::normalize(glm::rotate(direction, angle * speed, up));
    updateModelMatrix();
}

void GLObject::yaw(float angle, float speed) {
    if (flipY)
        angle = -angle;
    glm::vec3 right = modelMatrix[0];
    up = glm::normalize(glm::rotate(up, angle * speed, right));
    direction = glm::normalize(glm::cross(right, up));
    updateModelMatrix();
}

void GLObject::setModelMatrix(glm::mat4 transform) {
    this->modelMatrix = transform;
    this->normalMatrix = glm::inverseTranspose(this->modelMatrix);
    up = this->modelMatrix[1];
    direction = this->modelMatrix[2];
    position = this->modelMatrix[3];
}

void GLObject::draw(const std::function<void(GLObject *)> &fp) {

}

void GLObject::addTexture(Texture *texture) {
    this->textures.push_back(texture);
}

glm::mat4 GLObject::getModelMatrix() {
    return modelMatrix;
}

glm::mat4 GLObject::getNormalMatrix() {
    return normalMatrix;
}

void GLObject::updateModelMatrix() {
    glm::vec3 right = glm::normalize(glm::cross(up, direction));
    modelMatrix = glm::mat4({right.x, right.y, right.z, 0.0f},
                            {up.x, up.y, up.z, 0.0f},
                            {direction.x, direction.y, direction.z, 0.0f},
                            {position.x, position.y, position.z, 1.0f});
    setModelMatrix(modelMatrix);
}

GLObject::~GLObject() = default;

