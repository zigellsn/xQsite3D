#ifndef SDLTEST_SCENE_H
#define SDLTEST_SCENE_H

#include "xQsite3D.h"
#include "Mesh.h"
#include "Light.h"
#include "Texture.h"

class Scene {
public:
    explicit Scene();

    ~Scene();

    void loadFromFile(const string &filename);

    vector<Mesh *> getMeshes();

    Mesh *getMesh(unsigned int index);

    vector<Light *> getLights();

    Light *getLight(unsigned int index);

    void addMesh(Mesh *mesh);

    vector<Mesh *> additional_meshes;

private:
    void getAllMeshes(aiNode *node, aiMatrix4x4 transformation, vector<Mesh *> *meshes);

    void getAllLights(aiNode *node, aiMatrix4x4 transformation, vector<Light *> *lights);

    Mesh *getMesh(aiMesh *aimesh);

    Light *getLight(aiLight *ailight, int index);

    vector<Texture *> textures;
    const aiScene *scene;
};

#endif //SDLTEST_SCENE_H
