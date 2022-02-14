//
// Created by Simon on 04.02.2022.
//

#ifndef XQSITE_SHADERPROGRAM_H
#define XQSITE_SHADERPROGRAM_H

#include <map>
#include "xQsite3D.h"
#include <functional>

class ShaderProgram {
public:

    typedef struct blockValue {
        void *data;
        size_t size;
    } blockValue;

    typedef std::map<std::string, blockValue> block;

    ShaderProgram();

    virtual ~ShaderProgram();

    void compile(const std::string &vertexShaderPath, const std::string &fragmentShaderPath,
                 const std::string &geometryShaderPath = "");

    void link() const;

    void appendAttribute(const std::string &attribute);

    GLint getUniformLocation(const std::string &uniformName) const;

    GLint getUniformBlockIndex(const std::string &uniformName) const;

    void setUniformBlock(const GLchar *name, block &values);

    void invoke(const std::function<void(ShaderProgram *)> &fp);


private:
    static void compile(const std::string &filePath, GLuint id);

    GLuint programId;
    GLuint vertexShaderId;
    GLuint fragmentShaderId;
    GLuint geometryShaderId = 0;
    unsigned int numAttributes;
    unsigned int layout;
    std::map<std::string, GLuint> ubo;

};


#endif //XQSITE_SHADERPROGRAM_H
