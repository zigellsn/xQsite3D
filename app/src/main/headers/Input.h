//
// Created by Simon on 06.02.2022.
//

#ifndef SDLTEST_INPUT_H
#define SDLTEST_INPUT_H


#include "xQsite3D.h"
#include "State.h"

class Input {
public:
    Input(State *state);

    void ProcessInput();

private:
    State *state;
};


#endif //SDLTEST_INPUT_H
