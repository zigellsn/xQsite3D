//
// Created by Simon on 04.02.2022.
//

#include <iostream>
#include <cstdlib>
#include <SDL2/SDL.h>

#include "Error.h"
#include "GL/glew.h"

void error(const std::string &errorString) {
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

GLenum glCheckError_(const char *file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
            default:
                error = "UNKNOWN_ERROR";
                break;
        }
        std::cout << error << " (" << errorCode << ")" << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}