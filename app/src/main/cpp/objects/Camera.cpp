//
// Created by Simon on 08.02.2022.
//

#include "objects/Camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtx/rotate_vector.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up, glm::vec3 lookAt) : GLObject() {

    setPerspective(0.6911f);

    this->position = position;
    this->up = up;
    this->lookAt = lookAt;

    modelMatrix = glm::mat4(1.0f);
    viewMatrix = glm::mat4(1.0f);

    updateGlobals();
    updateViewMatrix();
}

Camera::Camera(aiNode *ainode, aiCamera *aicamera, const aiScene *scene)
        : GLObject() {
    name = ainode->mName.data;
    position = glm::vec3(aicamera->mPosition.x, aicamera->mPosition.y, aicamera->mPosition.z);
    up = glm::vec3(aicamera->mUp.x, aicamera->mUp.y, aicamera->mUp.z);
    lookAt = glm::vec3(aicamera->mLookAt.x, aicamera->mLookAt.y, aicamera->mLookAt.z);

    setPerspective(aicamera->mHorizontalFOV, aicamera->mAspect,
                   aicamera->mClipPlaneNear, aicamera->mClipPlaneFar);

    if (ainode == nullptr) {
        modelMatrix = glm::mat4(1);
    }
    aiMatrix4x4 m4;
    while (ainode != scene->mRootNode) {
        m4 = ainode->mTransformation * m4;
        ainode = ainode->mParent;
    }

    aiMatrix4x4 aim;
    aicamera->GetCameraMatrix(aim);
    m4 = aim * (ainode->mTransformation * m4);

    modelMatrix = glm::mat4(m4.a1, m4.b1, m4.c1, m4.d1,
                            m4.a2, m4.b2, m4.c2, m4.d2,
                            m4.a3, m4.b3, m4.c3, m4.d3,
                            m4.a4, m4.b4, m4.c4, m4.d4);

    updateGlobals();
    updateViewMatrix();
}

void Camera::updateViewMatrix() {
    viewMatrix = glm::lookAt(position, position - direction, up);
}

void Camera::updateGlobals() {
    glm::vec4 eG = modelMatrix * glm::vec4(position, 1.0f);
    position = glm::vec3(eG.x, eG.y, eG.z);

    glm::vec4 cG = modelMatrix * glm::vec4(lookAt, 0.0f);
    lookAt = glm::vec3(cG.x, cG.y, cG.z);

    glm::vec4 uG = modelMatrix * glm::vec4(up, 0.0f);
    up = glm::normalize(glm::vec3(uG.x, uG.y, uG.z));

    direction = -glm::normalize(lookAt - position);
}

void Camera::translate(const glm::vec3 moveDirection, float speed) {
    glm::vec3 right = {viewMatrix[0][0],
                       viewMatrix[1][0],
                       viewMatrix[2][0]}; // glm::normalize(glm::cross(up, direction));
    glm::mat3 newMatrix = glm::mat4(1.0f);
    if (mode == Mode::FLY) {
        newMatrix = glm::mat3(right, up, direction);
    } else {
        newMatrix = glm::mat3({1.0f, 0.0f, 0.0f},
                              {0.0f, 1.0f, 0.0f},
                              {direction.x, 0.0f, direction.z});
    }
    glm::vec3 nDirection = glm::normalize(moveDirection);
    nDirection = newMatrix * moveDirection;
    position += nDirection * speed;
    updateViewMatrix();
}

void Camera::setPerspective(float newFov, float newAspect, float newNear, float newFar) {
    this->fov = newFov;
    this->aspect = newAspect;
    this->near = newNear;
    this->far = newFar;
    projectionMatrix = glm::perspective(fov, aspect, near, far);
}

void Camera::roll(float angle, float speed) {
    up = glm::normalize(glm::rotate(up, angle * speed, direction));
    updateViewMatrix();
}

void Camera::pitch(float angle, float speed) {
    if (flipX)
        angle = -angle;
    direction = glm::normalize(glm::rotate(direction, angle * speed, up));
    updateViewMatrix();
}

void Camera::yaw(float angle, float speed) {
    if (flipY)
        angle = -angle;
    glm::vec3 right = {viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]};
    up = glm::normalize(glm::rotate(up, angle * speed, right));
    direction = glm::normalize(glm::cross(right, up));
    updateViewMatrix();
}
