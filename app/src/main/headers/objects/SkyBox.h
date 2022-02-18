//
// Created by Simon on 17.02.2022.
//

#ifndef XQSITE3D_SKYBOX_H
#define XQSITE3D_SKYBOX_H


#include "Mesh.h"

class SkyBox : public Mesh {
public:
    explicit SkyBox(const std::vector<std::string> &faces);

    explicit SkyBox(const std::string &faces);

    void setViewMatrix(glm::mat4 matrix);

    glm::mat4 viewMatrix = glm::mat4(1.0f);
protected:
private:
    void drawElements() override;

    void generateFaces();
};


#endif //XQSITE3D_SKYBOX_H
