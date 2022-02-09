//
// Created by Simon on 08.02.2022.
//

#include <iostream>
#include "objects/Camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtx/rotate_vector.hpp"

// constructor with vectors
Camera::Camera(glm::vec3 position, glm::vec3 up, glm::vec3 lookAt) : Front(glm::vec3(0.0f, 0.0f, 1.0f)),
                                                                           MovementSpeed(SPEED),
                                                                           MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
//    Position = position;
//    WorldUp = up;
//    Yaw = yaw;
//    Pitch = pitch;

    fov = 1.047f;
    near = 0.1f;
    far = 100.0f;
    aspect = 4.0f / 3.0f;
    projectionMatrix = glm::perspective(fov, aspect, near, far);

    this->position = position;
    this->up = up;
    this->lookAt = lookAt;

    modelMatrix = glm::mat4(1.0f);
    viewMatrix = glm::mat4(1.0f);

    updateGlobals();
    updateAxis();
    updateViewMatrix();
}

// constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(
        glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    // fov = 1.047f;
    near = 0.1f;
    far = 200.0f;
    aspect = 3.0f / 4.0f;
    updateCameraVectors();
}

Camera::Camera(aiNode *ainode, aiCamera *aicamera, const aiScene *scene) : Front(
        glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    name = ainode->mName.data;
    position = glm::vec3(aicamera->mPosition.x, aicamera->mPosition.y, aicamera->mPosition.z);
    up = glm::vec3(aicamera->mUp.x, aicamera->mUp.y, aicamera->mUp.z);
    lookAt = glm::vec3(aicamera->mLookAt.x, aicamera->mLookAt.y, aicamera->mLookAt.z);

    projectionMatrix = glm::perspective(aicamera->mHorizontalFOV, aicamera->mAspect,
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
    updateAxis();
    updateViewMatrix();

//    Yaw = glm::degrees(glm::half_pi<float>() - asin((glm::length(Front * glm::vec3(0.0f, 0.0f, 1.0f))) /
//                                           (glm::length(Front) * glm::length(glm::vec3(0.0f, 0.0f, 1.0f)))));
//
//    Pitch = glm::degrees(glm::half_pi<float>() - asin((glm::length(Front * glm::vec3(0.0f, 1.0f, 0.0f))) /
//                                                    (glm::length(Front) * glm::length(glm::vec3(0.0f, 1.0f, 0.0f)))));


//    fov = aicamera->mHorizontalFOV;
//    Zoom = glm::degrees(aicamera->mHorizontalFOV);
//    aspect = aicamera->mAspect;
//    far = aicamera->mClipPlaneFar;
//    near = aicamera->mClipPlaneNear;

//    Yaw = 63.3f;
//    Pitch = 46.7f;
    // updateCameraVectors(false);
}

void Camera::updateViewMatrix() {
    viewMatrix = glm::lookAt(position, lookAt, up);
    // projectionViewMatrix = projectionMatrix * viewMatrix;
}

void Camera::updateGlobals() {
    glm::vec4 eG = modelMatrix * glm::vec4(position, 0.0f);
    position = glm::vec3(eG.x, eG.y, eG.z);

    glm::vec4 cG = modelMatrix * glm::vec4(lookAt, 0.0f);
    lookAt = glm::vec3(cG.x, cG.y, cG.z);

    glm::vec4 uG = modelMatrix * glm::vec4(up, 0.0f);
    up = glm::vec3(uG.x, uG.y, uG.z);
}

void Camera::updateAxis() {
    yGlob = up;
    zGlob = lookAt - position;
    xGlob = glm::cross(yGlob, zGlob);
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() {
    return viewMatrix;
}

void Camera::translate(glm::vec3 translateVector)
{
    glm::vec3 l_z = translateVector.z*glm::normalize(zGlob);
    glm::vec3 l_y = translateVector.y*glm::normalize(yGlob);
    glm::vec3 l_x = translateVector.x*glm::normalize(xGlob);

    glm::vec3 t0 = l_z + l_y + l_x;

    position += t0;
    lookAt += t0;

    updateViewMatrix();
    updateAxis();
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
}

void Camera::roll(float angle) // local z
{
    glm::vec3 diffRotZ = glm::rotate(yGlob, angle, zGlob) - yGlob;
    up += diffRotZ;
    updateViewMatrix();
    updateAxis();
}

void Camera::pitch(float angle) // local X
{
    glm::vec3 diffCenter = glm::rotate(zGlob, -angle, xGlob) - zGlob;
    glm::vec3 diffUp = glm::rotate(yGlob, -angle, xGlob) - yGlob;
    lookAt += diffCenter;
    up += diffUp;
    updateViewMatrix();
    updateAxis();
}

void Camera::yaw(float angle) // local Y (up)
{
    glm::vec3 diffLR = glm::rotate(zGlob, -angle, yGlob) - zGlob;
    lookAt += diffLR;
    updateViewMatrix();
    updateAxis();
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    // updateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
    Zoom -= (float) yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors(bool calcFront) {
    // calculate the new Front vector
    if (calcFront) {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
    }
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front,
                                      WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}

glm::mat4 Camera::getProjectionMatrix() {
    return projectionMatrix;
}