#include "objects/Light.h"
#include "ShaderProgram.h"

Light::Light(aiNode *node, aiLight *light, const aiScene *scene) : GLObject() {
    name = light->mName.data;
    modelMatrix = glm::mat4();
    if (node == nullptr) {
        modelMatrix = glm::mat4(1);
    }
    aiMatrix4x4 m4;
    while (node != scene->mRootNode) {
        m4 = node->mTransformation * m4;
        node = node->mParent;
    }

    modelMatrix = glm::mat4(m4.a1, m4.b1, m4.c1, m4.d1,
                            m4.a2, m4.b2, m4.c2, m4.d2,
                            m4.a3, m4.b3, m4.c3, m4.d3,
                            m4.a4, m4.b4, m4.c4, m4.d4);

    this->ambient = glm::vec4(light->mColorAmbient.r,
                              light->mColorAmbient.g,
                              light->mColorAmbient.b,
                              1.0f);
    this->diffuse = glm::vec4(light->mColorDiffuse.r,
                              light->mColorDiffuse.g,
                              light->mColorDiffuse.b,
                              1.0f);
    this->specular = glm::vec4(light->mColorSpecular.r,
                               light->mColorSpecular.g,
                               light->mColorSpecular.b,
                               1.0f);
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

Light::~Light() = default;