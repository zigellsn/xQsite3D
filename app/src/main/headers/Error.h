//
// Created by Simon on 04.02.2022.
//

#pragma once

#include <string>
#include "GL/glew.h"

extern void error(const std::string& errorString);
extern void glError();
extern GLenum glCheckError_(const char *file, int line);

#define glCheckError() glCheckError_(__FILE__, __LINE__)