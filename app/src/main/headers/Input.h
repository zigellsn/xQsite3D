//
// Created by Simon on 06.02.2022.
//

#ifndef SDLTEST_INPUT_H
#define SDLTEST_INPUT_H


#include "xQsite3D.h"
#include "State.h"
#include "objects/Camera.h"

class Input {
public:
    Input(State *state);

    void ProcessInput(Camera *camera, const std::function<void(Uint32)> &fp = nullptr);

private:
    State *state;
    bool rmb = false;
};


#endif //SDLTEST_INPUT_H
