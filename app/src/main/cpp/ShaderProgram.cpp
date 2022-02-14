//
// Created by Simon on 04.02.2022.
//

#include <fstream>
#include "xQsite3D.h"
#include "ShaderProgram.h"

ShaderProgram::ShaderProgram() : programId(0), vertexShaderId(0), fragmentShaderId(0), numAttributes(0), layout(0) {

}

ShaderProgram::~ShaderProgram() = default;

void ShaderProgram::compile(const std::string &vertexShaderPath, const std::string &fragmentShaderPath,
                            const std::string &geometryShaderPath) {
    // Get a program object.
    programId = glCreateProgram();
    vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    if (vertexShaderId == 0) {
        error("Error creating vertex shader " + vertexShaderPath);
    }

    fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    if (fragmentShaderId == 0) {
        error("Error creating fragment shader " + fragmentShaderPath);
    }

    compile(vertexShaderPath, vertexShaderId);
    compile(fragmentShaderPath, fragmentShaderId);
    if (!geometryShaderPath.empty()) {
        geometryShaderId = glCreateShader(GL_GEOMETRY_SHADER);
        if (fragmentShaderId == 0) {
            error("Error creating geometry shader " + fragmentShaderPath);
        }
        compile(geometryShaderPath, geometryShaderId);
    }
}

void ShaderProgram::compile(const std::string &filePath, GLuint id) {
    std::ifstream file(filePath);
    std::stringstream stream;
    // file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    std::string fileContent;
    try {
        file.open(filePath);
        stream << file.rdbuf();
        file.close();
        fileContent = stream.str();
    } catch (std::ifstream::failure &e) {
        perror(filePath.c_str());
        error("Error reading file " + filePath);
    }

    const char *contentPtr = fileContent.c_str();

    // Get strings for glShaderSource.
    glShaderSource(id, 1, &contentPtr, nullptr);

    glCompileShader(id);

    GLint isCompiled = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(id); // Don't leak the shader.

        std::printf("%s\n", &(errorLog[0]));
        error("Compilation of " + filePath + " failed.");
    }
}

void ShaderProgram::link() const {
    // Vertex and fragment shaders are successfully compiled.
    // Now time to link them together into a program.

    // Attach our shaders to our program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    if (geometryShaderId != 0)
        glAttachShader(programId, geometryShaderId);

    // Link our program
    glLinkProgram(programId);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(programId, GL_LINK_STATUS, (int *) &isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(programId, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(programId);
        // Don't leak shaders either.
        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);
        if (geometryShaderId != 0)
            glDeleteShader(geometryShaderId);

        // Use the infoLog as you see fit.

        std::printf("%s\n", &(infoLog[0]));
        error("Linking of shader program failed.");
        return;
    }

    // Always detach shaders after a successful link.
    glDetachShader(programId, vertexShaderId);
    glDetachShader(programId, fragmentShaderId);
    if (geometryShaderId != 0)
        glDetachShader(programId, geometryShaderId);
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
    if (geometryShaderId != 0)
        glDeleteShader(geometryShaderId);
}

void ShaderProgram::appendAttribute(const std::string &attribute) {
    glBindAttribLocation(programId, numAttributes++, attribute.c_str());
}

void ShaderProgram::invoke(const std::function<void(ShaderProgram *)> &fp) {
    glUseProgram(programId);
    for (int i = 0; i < numAttributes; i++) {
        glEnableVertexAttribArray(i);
    }
    fp(this);
    glUseProgram(0);
    for (int i = 0; i < numAttributes; i++) {
        glDisableVertexAttribArray(i);
    }
    layout = 0;
}

GLint ShaderProgram::getUniformLocation(const std::string &uniformName) const {
    GLint location = glGetUniformLocation(programId, uniformName.c_str());
    if (location == GL_INVALID_INDEX) {
        error("Uniform " + uniformName + " not found in Shader");
    }
    return location;
}

GLint ShaderProgram::getUniformBlockIndex(const std::string &uniformName) const {
    GLint index = glGetUniformBlockIndex(programId, uniformName.c_str());
    if (index == GL_INVALID_INDEX) {
        error("Uniform " + uniformName + " not found in Shader");
    }
    return index;
}

void ShaderProgram::setUniformBlock(const GLchar *name, block &values) {
    int countData = (int) values.size();

    std::vector<const GLchar *> keys;
    for (const auto &imap: values)
        keys.push_back((const GLchar *) imap.first.c_str());

    auto *names = new GLchar const *[countData];
    std::copy(keys.begin(), keys.end(), names);

    GLuint uboHandle;
    auto it = ubo.find(name);
    if (it == ubo.end()) {
        glGenBuffers(1, &uboHandle);
        ubo[name] = uboHandle;
    } else {
        uboHandle = it->second;
    }
    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);

    GLuint blockIndex = glGetUniformBlockIndex(programId, name);

    auto *uniformIndices = new GLuint[countData];
    glGetUniformIndices(programId, countData, names, uniformIndices);

    auto *uniformOffsets = new GLint[countData];
    glGetActiveUniformsiv(programId, countData, uniformIndices,
                          GL_UNIFORM_OFFSET, uniformOffsets);

    GLsizei blockSize(0);
    glGetActiveUniformBlockiv(programId, blockIndex,
                              GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

    auto *blockBuffer = (GLubyte *) malloc(blockSize);

    int i = 0;
    for (auto &value: values) {
        int sz = (int) value.second.size;
        memcpy(blockBuffer + uniformOffsets[i], value.second.data, sz);
        i++;
    }

    glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);
    glUniformBlockBinding(programId, blockIndex, layout);
    glBindBufferBase(GL_UNIFORM_BUFFER, layout, uboHandle);

    free(blockBuffer);
    layout++;
    delete[] uniformIndices;
    delete[] uniformOffsets;
}
