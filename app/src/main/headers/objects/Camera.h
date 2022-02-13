//
// Created by Simon on 08.02.2022.
//

#ifndef XQSITE3D_CAMERA_H
#define XQSITE3D_CAMERA_H

#include "glm/vec3.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "assimp/scene.h"
#include "GLObject.h"

class Camera : GLObject {
public:

    enum class Mode {
        FLY,
        WALK
    };

    constexpr static const glm::vec3 FORWARD = {0.0f, 0.0f, -1.0f};
    constexpr static const glm::vec3 BACKWARD = {0.0f, 0.0f, 1.0f};
    constexpr static const glm::vec3 LEFT = {-1.0f, 0.0f, 0.0f};
    constexpr static const glm::vec3 RIGHT = {1.0f, 0.0f, 0.0f};
    constexpr static const glm::vec3 UP = {0.0f, 1.0f, 0.0f};
    constexpr static const glm::vec3 DOWN = {0.0f, -1.0f, 0.0f};

    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    glm::vec3 lookAt;
    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 direction;

    float fov;
    float aspect;
    float near;
    float far;

    Mode mode = Mode::FLY;

    bool flipX = false;
    bool flipY = false;

    std::string name;

    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, -15.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                    glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, 1.0f));

    Camera(aiNode *ainode, aiCamera *aicamera, const aiScene *scene = nullptr);

    void setPerspective(float newFov, float newAspect = 4.0f / 3.0f, float newNear = 0.1f, float newFar = 100.0f);

    void translate(glm::vec3 moveDirection, float speed = 1.0f);

    void roll(float angle, float speed = 1.0f);

    void yaw(float angle, float speed = 1.0f);

    void pitch(float angle, float speed = 1.0f);

private:
    void updateViewMatrix();

    void updateGlobals();
};

#endif //XQSITE3D_CAMERA_H
