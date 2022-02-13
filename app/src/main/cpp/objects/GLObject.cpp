#include "objects/GLObject.h"

GLObject::GLObject(const std::string &name) : material(nullptr), modelMatrix(glm::mat4(1.0f)) {
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

void GLObject::draw() {

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

glm::vec4 GLObject::getDiffuseColor() {
    return getColor(AI_MATKEY_COLOR_DIFFUSE);
}

glm::vec4 GLObject::getSpecularColor() {
    return getColor(AI_MATKEY_COLOR_SPECULAR);
}

glm::vec4 GLObject::getEmissiveColor() {
    return getColor(AI_MATKEY_COLOR_EMISSIVE);
}

glm::vec4 GLObject::getAmbientColor() {
    return getColor(AI_MATKEY_COLOR_AMBIENT);
}

glm::vec4 GLObject::getColor(const char *key, unsigned int type,
                             unsigned int idx) {
    aiColor4D color;
    if (this->material == nullptr) {
        return glm::vec4(1.0f);
    }
    if (this->material->Get(key, type, idx, color) == AI_SUCCESS) {
        glm::vec4 col = glm::vec4(color.r, color.g, color.b, color.a);
        return col;
    }
    return glm::vec4(1.0f);
}

glm::mat4 GLObject::getModelMatrix() {
    return modelMatrix;
}

GLObject::~GLObject() = default;

