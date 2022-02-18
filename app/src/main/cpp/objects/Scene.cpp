#include "objects/Scene.h"
#include "xQsite3D.h"

Scene::Scene() : scene(nullptr) {
}

void Scene::loadFromFile(const string &filename, int importMode) {
    this->mode = importMode;

    blenderCorrectionMatrix = glm::rotate(blenderCorrectionMatrix, -glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
    blenderCorrectionMatrix = glm::rotate(blenderCorrectionMatrix, glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));

    auto *importer = new Assimp::Importer;
    scene = importer->ReadFile(filename,
                               aiProcess_Triangulate | aiProcess_GenSmoothNormals
                               | aiProcess_FixInfacingNormals
                               | aiProcess_GenUVCoords | aiProcess_GenBoundingBoxes
                               | aiProcess_ImproveCacheLocality | aiProcess_SortByPType);
}

void Scene::addMesh(Mesh *mesh) {
    additional_meshes[mesh->name] = mesh;
}

std::map<std::string, GLObject *> Scene::getMeshes() {
    std::map<std::string, GLObject *> meshes;
    if (scene != nullptr) {
        getAllMeshes(scene->mRootNode, scene->mRootNode->mTransformation,
                     &meshes);
    }
    return meshes;
}

void Scene::getAllMeshes(aiNode *node, aiMatrix4x4 transformation,
                         std::map<std::string, GLObject *> *meshes) {
    aiMatrix4x4 transform;
    Mesh *tmpMesh;
    if (node->mNumMeshes > 0) {
        Mesh *tmpParentMesh = new Mesh(node->mName.data);
        transform = node->mTransformation * transformation;
        tmpParentMesh->setModelMatrix(
                glm::mat4(transform.a1, transform.b1, transform.c1, transform.d1,
                          transform.a2, transform.b2, transform.c2, transform.d2,
                          transform.a3, transform.b3, transform.c3, transform.d3,
                          transform.a4, transform.b4, transform.c4, transform.d4));
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            tmpMesh = getMesh(node->mMeshes[i], node);
            transform = node->mTransformation * transformation;
            if (tmpMesh != nullptr) {
                tmpMesh->setModelMatrix(
                        glm::mat4(transform.a1, transform.b1, transform.c1, transform.d1,
                                  transform.a2, transform.b2, transform.c2, transform.d2,
                                  transform.a3, transform.b3, transform.c3, transform.d3,
                                  transform.a4, transform.b4, transform.c4, transform.d4));
                tmpParentMesh->children[tmpMesh->name] = tmpMesh;
                for (unsigned int j = 0; j < node->mNumChildren; j++) {
                    getAllMeshes(node->mChildren[j], transform, &tmpMesh->children);
                }
            }
        }
        meshes->insert(std::pair<std::string, Mesh *>(tmpParentMesh->name, tmpParentMesh));
    } else {
        transform = node->mTransformation * transformation;
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            getAllMeshes(node->mChildren[i], transform, meshes);
        }
    }

    meshes->insert(additional_meshes.begin(), additional_meshes.end());
}

Mesh *Scene::getMesh(unsigned int index, aiNode *node) {
    if (scene != nullptr) {
        if (index < scene->mNumMeshes) {
            if (mode & BLENDER) {
                for (int i = 0; i < scene->mMeshes[index]->mNumVertices; i++) {
                    glm::vec4 vert = {scene->mMeshes[index]->mVertices[i].x,
                                      scene->mMeshes[index]->mVertices[i].y,
                                      scene->mMeshes[index]->mVertices[i].z,
                                      1.0f};
                    vert = blenderCorrectionMatrix * vert;
                    scene->mMeshes[index]->mVertices[i] = aiVector3D(vert.x, vert.y, vert.z);
                }
                for (int i = 0; i < scene->mMeshes[index]->mNumVertices; i++) {
                    glm::vec4 vert = {scene->mMeshes[index]->mNormals[i].x,
                                      scene->mMeshes[index]->mNormals[i].y,
                                      scene->mMeshes[index]->mNormals[i].z,
                                      1.0f};
                    vert = blenderCorrectionMatrix * vert;
                    scene->mMeshes[index]->mNormals[i] = aiVector3D(vert.x, vert.y, vert.z);
                }
                for (int j = 0; j < scene->mMeshes[index]->GetNumUVChannels(); j++) {
                    for (int i = 0; i < scene->mMeshes[index]->mNumVertices; i++) {
                        if (scene->mMeshes[index]->mTextureCoords[j] != nullptr)
                            scene->mMeshes[index]->mTextureCoords[j][i].y =
                                    1.0f - scene->mMeshes[index]->mTextureCoords[j][i].y;
                    }
                }
                glm::vec4 min = {scene->mMeshes[index]->mAABB.mMin.x,
                                 scene->mMeshes[index]->mAABB.mMin.y,
                                 scene->mMeshes[index]->mAABB.mMin.z,
                                 1.0f};
                min = blenderCorrectionMatrix * min;
                scene->mMeshes[index]->mAABB.mMin = aiVector3D(min.x, min.y, min.z);
                glm::vec4 max = {scene->mMeshes[index]->mAABB.mMax.x,
                                 scene->mMeshes[index]->mAABB.mMax.y,
                                 scene->mMeshes[index]->mAABB.mMax.z,
                                 1.0f};
                max = blenderCorrectionMatrix * max;
                scene->mMeshes[index]->mAABB.mMax = aiVector3D(max.x, max.y, max.z);
            }
            Mesh *tmpMesh = new Mesh(node, scene->mMeshes[index], scene);
            return tmpMesh;
        }
    }
    return nullptr;
}

std::map<std::string, Camera *> Scene::getCameras() {
    std::map<std::string, Camera *> cameras;

    if (scene != nullptr) {
        for (unsigned int i = 0; i < scene->mNumCameras; i++) {
            Camera *tmpCamera = getCamera(i);
            cameras[tmpCamera->name] = tmpCamera;
        }
    }
    return cameras;
}

Camera *Scene::getCamera(unsigned int index) {
    if (scene != nullptr) {
        if (index < scene->mNumCameras) {
            aiNode *cameraNode = scene->mRootNode->FindNode(scene->mCameras[index]->mName);
            return new Camera(cameraNode, scene->mCameras[index], scene);
        }
    }
    return nullptr;

}

std::map<std::string, Light *> Scene::getLights() {
    std::map<std::string, Light *> lights;

    if (scene != nullptr) {
        for (unsigned int i = 0; i < scene->mNumLights; i++) {
            Light *tmpLight = getLight(i);
            if (tmpLight != nullptr)
                lights[tmpLight->name] = tmpLight;
        }
    }
    return lights;
}

Light *Scene::getLight(unsigned int index) {
    if (scene != nullptr) {
        if (index < scene->mNumLights && index <= GL_MAX_LIGHTS - 1) {
            aiLight *light = scene->mLights[index];
            aiNode *node = scene->mRootNode->FindNode(light->mName);
            return new Light(node, light, scene);
        }
    }
    return nullptr;
}

Scene::~Scene() = default;
