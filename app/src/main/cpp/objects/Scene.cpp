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
    additional_meshes.push_back(mesh);
}

vector<Mesh *> Scene::getMeshes() {
    vector<Mesh *> meshes;
    // mesh *tmpMesh = nullptr;
    if (scene != nullptr) {
        getAllMeshes(scene->mRootNode, scene->mRootNode->mTransformation,
                     &meshes);
    }
    return meshes;
}

void Scene::getAllMeshes(aiNode *node, aiMatrix4x4 transformation,
                         vector<Mesh *> *meshes) {
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
            tmpMesh = getMesh(node->mMeshes[i]);
            transform = node->mTransformation * transformation;
            if (tmpMesh != nullptr) {
                tmpMesh->setModelMatrix(
                        glm::mat4(transform.a1, transform.b1, transform.c1, transform.d1,
                                  transform.a2, transform.b2, transform.c2, transform.d2,
                                  transform.a3, transform.b3, transform.c3, transform.d3,
                                  transform.a4, transform.b4, transform.c4, transform.d4));
                tmpParentMesh->children.push_back(tmpMesh);
                for (unsigned int j = 0; j < node->mNumChildren; j++) {
                    getAllMeshes(node->mChildren[j], transform, &tmpMesh->children);
                }
            }
        }
        meshes->push_back(tmpParentMesh);
    } else {
        transform = node->mTransformation * transformation;
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            getAllMeshes(node->mChildren[i], transform, meshes);
        }
    }

    meshes->insert(meshes->end(), additional_meshes.begin(), additional_meshes.end());
}

Mesh *Scene::getMesh(unsigned int index) {
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
                    // TODO: Transform UV Coordinates
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
            Mesh *tmpMesh = new Mesh(scene->mMeshes[index], scene);
            return tmpMesh;
        }
    }
    return nullptr;
}

Mesh *Scene::getMesh(aiMesh *aimesh) {
    if (scene != nullptr) {
        Mesh *tmpMesh = new Mesh(aimesh, scene);
        return tmpMesh;
    }
    return nullptr;
}

vector<Camera *> Scene::getCameras() {
    vector<Camera *> cameras;

    if (scene != nullptr) {
        for (unsigned int i = 0; i < scene->mNumCameras; i++) {
            Camera *tmpCamera = getCamera(i);
            cameras.push_back(tmpCamera);
        }
    }
    return cameras;
}

Camera *Scene::getCamera(unsigned int index) {
    if (scene != nullptr) {
        if (index < scene->mNumCameras) {
            aiNode *cameraNode = scene->mRootNode->FindNode(scene->mCameras[index]->mName);
            auto *tmpCamera = new Camera(cameraNode, scene->mCameras[index], scene);
            return tmpCamera;
        }
    }
}

Camera *Scene::getCamera(aiNode *ainode, aiCamera *aicamera, int index) {
    if (scene != nullptr) {
        auto *tmpCamera = new Camera(ainode, aicamera, scene);
        return tmpCamera;
    }
    return nullptr;
}

vector<Light *> Scene::getLights() {
    vector<Light *> lights;

    if (scene != nullptr) {
        for (unsigned int i = 0; i < scene->mNumLights; i++) {
            Light *tmpLight = getLight(i);
            if (tmpLight != nullptr)
                lights.push_back(tmpLight);
        }
    }
    return lights;
}

Light *Scene::getLight(unsigned int index) {
    if (scene != nullptr) {
        if (index < scene->mNumLights && index <= GL_MAX_LIGHTS - 1) {
            aiNode *node = scene->mRootNode->FindNode(
                    scene->mLights[index]->mName);
            auto *tmpLight = new Light(glm::vec4(node->mTransformation.a4, node->mTransformation.b4,
                                                 node->mTransformation.c4, node->mTransformation.d4));
            tmpLight->setAmbient(glm::vec4(scene->mLights[index]->mColorAmbient.r,
                                           scene->mLights[index]->mColorAmbient.g,
                                           scene->mLights[index]->mColorAmbient.b, 1.0f));
            tmpLight->setDiffuse(glm::vec4(scene->mLights[index]->mColorDiffuse.r,
                                           scene->mLights[index]->mColorDiffuse.g,
                                           scene->mLights[index]->mColorDiffuse.b, 1.0f));
            tmpLight->setSpecular(glm::vec4(scene->mLights[index]->mColorSpecular.r,
                                            scene->mLights[index]->mColorSpecular.g,
                                            scene->mLights[index]->mColorSpecular.b, 1.0f));
            return tmpLight;
        }
    }
    return nullptr;
}

Light *Scene::getLight(aiLight *ailight, int index) {
    if (scene != nullptr) {
        auto *tmpLight = new Light(glm::vec4(ailight->mPosition.x,
                                             ailight->mPosition.y, ailight->mPosition.z, 1.0f));
        tmpLight->setAmbient(glm::vec4(ailight->mColorAmbient.r, ailight->mColorAmbient.g,
                                       ailight->mColorAmbient.b, 1.0f));
        tmpLight->setDiffuse(glm::vec4(ailight->mColorDiffuse.r, ailight->mColorDiffuse.g,
                                       ailight->mColorDiffuse.b, 1.0f));
        tmpLight->setSpecular(glm::vec4(ailight->mColorSpecular.r,
                                        ailight->mColorSpecular.g, ailight->mColorSpecular.b, 1.0f));
        return tmpLight;
    }
    return nullptr;
}

void Scene::getAllLights(aiNode *node, aiMatrix4x4 transformation, vector<Light *> *lights) {

}

Scene::~Scene() = default;
