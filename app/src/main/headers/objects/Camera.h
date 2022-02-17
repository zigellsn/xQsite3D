//
// Created by Simon on 08.02.2022.
//

#ifndef XQSITE3D_CAMERA_H
#define XQSITE3D_CAMERA_H

#include "glm/vec3.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "assimp/scene.h"
#include "GLObject.h"

class Camera : public GLObject {
public:

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    glm::vec3 lookAt;

    float fov;
    float aspect;
    float near;
    float far;

    std::string name;

    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, -15.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                    glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, 1.0f));

    Camera(aiNode *node, aiCamera *camera, const aiScene *scene = nullptr);

    void setPerspective(float newFov, float newAspect = 4.0f / 3.0f, float newNear = 0.1f, float newFar = 100.0f);

    void translate(glm::vec3 moveDirection, float speed) override;

    void roll(float angle, float speed) override;

    void yaw(float angle, float speed) override;

    void pitch(float angle, float speed) override;

private:
    void updateViewMatrix();

    void updateGlobals();
};

#endif //XQSITE3D_CAMERA_H
