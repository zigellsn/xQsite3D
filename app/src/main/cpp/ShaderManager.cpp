//
// Created by Simon on 07.02.2022.
//

#include "ShaderManager.h"

void ShaderManager::addShader(const std::string &name, ShaderProgram *shader) {
    shaders[name] = shader;
}

ShaderProgram *ShaderManager::createShader(const std::string &name, const std::string &vertexShaderPath,
                                           const std::string &fragmentShaderPath,
                                           const std::string &geometryShaderPath) {
    shaders[name] = new ShaderProgram();
    shaders[name]->compile(vertexShaderPath, fragmentShaderPath, geometryShaderPath);
    return shaders[name];
}

ShaderProgram *ShaderManager::getShader(const std::string &name) {
    return shaders[name];
}

ShaderManager::~ShaderManager() {
    for (auto &i : shaders) {
        delete i.second;
    }
}
