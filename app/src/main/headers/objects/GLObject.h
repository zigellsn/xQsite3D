#ifndef XQSITE_GL_OBJECT_H
#define XQSITE_GL_OBJECT_H

#include "xQsite3D.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "glm/gtx/rotate_vector.hpp"
#include <functional>

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

class GLObject {
public:
    explicit GLObject(const std::string &name = "");

    virtual ~GLObject();

    virtual void setModelMatrix(glm::mat4 transform);

    virtual void translate(glm::vec3 moveDirection, float speed);

    virtual void roll(float angle, float speed);

    virtual void yaw(float angle, float speed);

    virtual void pitch(float angle, float speed);

    virtual glm::mat4 getModelMatrix();

    virtual glm::mat4 getNormalMatrix();

    virtual glm::vec4 getPosition();

    virtual void draw(const std::function<void(GLObject *)> &fp);

    virtual void addTexture(Texture *texture);

    glm::vec3 position{};
    glm::vec3 up{};
    glm::vec3 direction{};

    bool flipX = false;
    bool flipY = false;
    std::string name;

protected:
    glm::mat4 modelMatrix{};
    glm::mat4 normalMatrix{};

    std::vector<Texture *> textures;

    virtual void drawElements() {};

private:
    void updateModelMatrix();
};


#endif //XQSITE_GL_OBJECT_H
