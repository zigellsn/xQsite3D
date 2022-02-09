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
    float speed = 0.5f;
    float mouseSpeed = 0.0005f;
    int delay = 0;

    float frameTime = 0.0f;
    int currentFrame = 0;
    bool debug = false;
    GState state = GState::STOPPED;

    float maxFPS = 60.0f;
    GLfloat fps = 0.0f;
};


#endif //SDLTEST_STATE_H
