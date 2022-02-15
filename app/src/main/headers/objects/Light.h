#ifndef XQSITE_LIGHT_H
#define XQSITE_LIGHT_H

#include "GLObject.h"
#include "xQsite3D.h"

class Light : public GLObject {
public:
    explicit Light(aiNode *ainode, aiLight *ailight, const aiScene *scene);

    ~Light() override;

    glm::vec4 ambient{};

    glm::vec4 specular{};

    glm::vec4 diffuse{};

    void setAmbient(glm::vec4 color);

    void setSpecular(glm::vec4 color);

    void setDiffuse(glm::vec4 color);
};

#endif //XQSITE_LIGHT_H
