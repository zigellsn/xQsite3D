//
// Created by Simon on 06.02.2022.
//

#ifndef SDLTEST_STATE_H
#define SDLTEST_STATE_H

#include "xQsite3D.h"

enum class GState {
    RUNNING, STOPPED
};

class State {
public:
    glm::mat4 projectionMatrix{glm::mat4(1.0f)}, viewMatrix{glm::mat4(1.0f)};

    // position
    struct Camera {
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 15.0f);
        glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec2 rotation = glm::vec2(0.0f, 0.0f);
    } camera;
    // Initial Field of View
    float speed = 0.5f;
    float mouseSpeed = 0.0005f;
    unsigned long lastTime = 0;
    int delay = 0;

    float frameTime = 0.0f;
    int currentFrame = 0;
    float initialFoV = 1.047f;
    float aspect = 3.0f / 4.0f;
    bool debug = false;
    GState state = GState::STOPPED;

    float maxFPS = 60.0f;
    GLfloat fps = 0.0f;
};


#endif //SDLTEST_STATE_H
