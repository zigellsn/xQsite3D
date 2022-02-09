//
// Created by Simon on 08.02.2022.
//

#ifndef XQSITE3D_CAMERA_H
#define XQSITE3D_CAMERA_H

#include "glm/vec3.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "assimp/scene.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = 90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
// camera Attributes
    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    glm::vec3 lookAt;
    glm::vec3 position;
    glm::vec3 up;

    glm::vec3 xGlob;
    glm::vec3 yGlob;
    glm::vec3 zGlob;

    // euler Angles
    float Yaw;
    float Pitch;
// camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    float fov;
    float aspect;
    float near;
    float far;

    std::string name;

// constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, -15.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, -1.0f));

// constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    Camera(aiNode *ainode, aiCamera *aicamera, const aiScene *scene = nullptr);

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    glm::mat4 getProjectionMatrix();

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

    void translate(glm::vec3 translateVector);
    void roll(float angle);
    void yaw(float angle);
    void pitch(float angle);

private:
// calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors(bool calcFront = true);

    void updateViewMatrix();
    void updateGlobals();
    void updateAxis();
};

#endif //XQSITE3D_CAMERA_H
