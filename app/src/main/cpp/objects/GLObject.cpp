#include "objects/GLObject.h"

GLObject::GLObject(const std::string &name) : modelMatrix(glm::mat4(1.0f)) {
    this->name = name;
}

void GLObject::translate(const glm::vec3 vector) {
    modelMatrix = glm::translate(modelMatrix, vector);
}

glm::vec4 GLObject::getPosition() {
    glm::vec4 ret = this->modelMatrix[3];
    return ret;
}

void GLObject::rotate(int axis, float angle) {
    glm::vec3 v_axis;
    if (axis == X_AXIS) {
        v_axis = glm::vec3(1.0f, 0.0f, 0.0f);
    } else if (axis == Y_AXIS) {
        v_axis = glm::vec3(0.0f, 1.0f, 0.0f);
    } else if (axis == Z_AXIS) {
        v_axis = glm::vec3(0.0f, 0.0f, 1.0f);
    } else
        return;

    modelMatrix = glm::rotate(modelMatrix, angle, v_axis);
}

/*void GLObject::rotate(glm::vec4, float angle) {
	//utils::RotateAboutVertex(&this->modelMatrix, vertex, angle);
}*/

void GLObject::setModelMatrix(glm::mat4 transform) {
    this->modelMatrix = transform;
}

glm::vec4 GLObject::getRotation() {
    glm::vec4 ret = glm::vec4();
    /*ret[0] = this->rx;
     ret[1] = this->ry;
     ret[2] = this->rz;*/
    return ret;
}

void GLObject::draw(const std::function<void(GLObject *)> &fp) {

}

void GLObject::addTexture(Texture *texture) {
    this->textures.push_back(texture);
}

void GLObject::scale(glm::vec3 vector) {
    this->modelMatrix = glm::scale(this->modelMatrix, vector);
    //utils::Scale(&this->modelMatrix, x, y, z);
}

glm::vec4 GLObject::getScale() {
    glm::vec4 ret = glm::vec4();
    /*ret[0] = this->sx;
     ret[1] = this->sy;
     ret[2] = this->sz;*/
    return ret;
}

glm::mat4 GLObject::getModelMatrix() {
    return modelMatrix;
}

GLObject::~GLObject() = default;

