#ifndef XQSITE_GL_OBJECT_H
#define XQSITE_GL_OBJECT_H

#include "xQsite3D.h"
#include "Texture.h"
#include "ShaderProgram.h"

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

class GLObject {
public:
    explicit GLObject(const std::string &name = "");

    virtual ~GLObject();

    virtual void setModelMatrix(glm::mat4 transform);

    virtual void translate(glm::vec3 vector);

    virtual void scale(glm::vec3 vector);

    virtual void rotate(int axis, float angle);

    //void rotate(glm::vec4 vector, float angle);
    virtual glm::mat4 getModelMatrix();

    virtual glm::vec4 getPosition();

    virtual glm::vec4 getScale();

    virtual glm::vec4 getRotation();

    virtual glm::vec4 getColor(const char *key, unsigned int type, unsigned int idx);

    virtual glm::vec4 getDiffuseColor();

    virtual glm::vec4 getAmbientColor();

    virtual glm::vec4 getSpecularColor();

    virtual glm::vec4 getEmissiveColor();

    virtual void draw();

    virtual void addTexture(Texture *texture);

protected:
    std::string name;
    glm::mat4 modelMatrix{};
    aiMaterial *material;

    std::vector<Texture *> textures;

    virtual void drawElements() {};
};


#endif //XQSITE_GL_OBJECT_H
