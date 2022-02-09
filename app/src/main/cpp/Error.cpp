//
// Created by Simon on 04.02.2022.
//

#include <iostream>
#include <cstdlib>
#include <SDL2/SDL.h>

#include "Error.h"
#include "GL/glew.h"

void error(const std::string& errorString) {
    std::cout << errorString << std::endl;
    SDL_Quit();
    std::exit(0);
}

void glError() {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << err << std::endl;
    }
}