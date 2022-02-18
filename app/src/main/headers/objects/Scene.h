#ifndef SDLTEST_SCENE_H
#define SDLTEST_SCENE_H

#include "xQsite3D.h"
#include "Mesh.h"
#include "Light.h"
#include "Texture.h"
#include "Camera.h"

const int BLENDER = 1;

class Scene {
public:

    explicit Scene();

    ~Scene();

    void loadFromFile(const string &filename, int importMode = 0);

    std::map<std::string, GLObject *> getMeshes();

    Mesh *getMesh(unsigned int index, aiNode *node);

    std::map<std::string, Camera *> getCameras();

    Camera *getCamera(unsigned int index);

    std::map<std::string, Light *> getLights();

    Light *getLight(unsigned int index);

    void addMesh(Mesh *mesh);

    std::map<std::string, Mesh *> additional_meshes;

private:
    void getAllMeshes(aiNode *node, aiMatrix4x4 transformation, std::map<std::string, GLObject *> *meshes);

    const aiScene *scene;

    glm::mat4 blenderCorrectionMatrix = glm::mat4(1.0f);

    int mode = 0;
};

#endif //SDLTEST_SCENE_H
