#ifndef XQSITE_LIGHT_H
#define XQSITE_LIGHT_H

#include "GLObject.h"
#include "xQsite3D.h"

class Light : public GLObject {
public:
    explicit Light(aiNode *node, aiLight *light, const aiScene *scene);

    ~Light() override;

    glm::vec4 ambient{};

    glm::vec4 specular{};

    glm::vec4 diffuse{};

    void setAmbient(glm::vec4 color);

    void setSpecular(glm::vec4 color);

    void setDiffuse(glm::vec4 color);

    glm::mat4 lookAt{};
};

#endif //XQSITE_LIGHT_H
