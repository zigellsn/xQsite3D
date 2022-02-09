//
// Created by Simon on 07.02.2022.
//

#ifndef XQSITE_SHADERMANAGER_H
#define XQSITE_SHADERMANAGER_H

#include "xQsite3D.h"
#include "ShaderProgram.h"

class ShaderManager {
public:
    virtual ~ShaderManager();

    void addShader(const std::string &name, ShaderProgram *shader);

    ShaderProgram *getShader(const std::string &name);

    ShaderProgram *
    createShader(const std::string &name, const std::string &vertexShaderPath, const std::string &fragmentShaderPath,
                 const std::string &geometryShaderPath = "");

private:
    std::map<std::string, ShaderProgram *> shaders;
};


#endif //XQSITE_SHADERMANAGER_H
