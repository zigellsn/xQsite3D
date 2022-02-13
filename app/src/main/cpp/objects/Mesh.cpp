#include "objects/Mesh.h"

#include <utility>

Mesh::Mesh(const std::string &name) : GLObject(name), indices(), positionBuffer(0), colorBuffer(0), normalBuffer(0),
                                      textureCoordinateBuffer(0),
                                      indexBuffer(0), vao(0), meshData() {
    glGenVertexArrays(1, &vao);
}

Mesh::Mesh(aiMesh *mesh, const aiScene *scene) : Mesh() {
    aiVector3D *textureCoordinates;
    string dir = "res/textures/";
    this->name = mesh->mName.data;
    this->material = scene->mMaterials[mesh->mMaterialIndex];

    unsigned int uvChannels = mesh->GetNumUVChannels();
    textureCoordinates = nullptr;
    for (int j = 0; j < uvChannels; j++) {
        textureCoordinates = mesh->mTextureCoords[j];
        unsigned int textureCount = material->GetTextureCount(aiTextureType_DIFFUSE);
        for (int i = 0; i < textureCount; i++) {
            aiString Path;
            if (material->GetTexture(aiTextureType_DIFFUSE, i, &Path, nullptr,
                                     nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS) {
                string path = Path.data;
                string base_filename = path.substr(path.find_last_of("/\\") + 1);
                string FullPath = dir + base_filename;
                SDL_Surface *surface = IMG_Load(FullPath.c_str());
                textures.push_back(textureFromSurface(surface));
                SDL_FreeSurface(surface);
            }
        }
    }
    setIndices(mesh->mNumFaces, mesh->mFaces);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        meshData.position.push_back({mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f});
        meshData.normal.push_back({mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, 1.0f});
        if (textureCoordinates != nullptr) {
            meshData.textureCoordinate.push_back({textureCoordinates[i].x, textureCoordinates[i].y /*, true*/});
        }
//        if (mesh->mColors[i] != nullptr) {
//            meshData.dColor.push_back(
//                    {mesh->mColors[i]->r, mesh->mColors[i]->g, mesh->mColors[i]->b, mesh->mColors[i]->a});
//        }
    }
    prepareBuffers();
}

void Mesh::setVertexData(Vertex vertexBuffer) {
    this->meshData = std::move(vertexBuffer);
    prepareBuffers();
}

void Mesh::setIndices(std::vector<GLuint> indexData) {
    this->indices = indexData;
    glBindVertexArray(vao);
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indexData.size(), &indexData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::setIndices(unsigned int numFaces, const aiFace *faces) {
    std::vector<GLuint> indexData;
    for (unsigned int i = 0; i < numFaces; i++) {
        for (unsigned int j = 0; j < faces->mNumIndices; j++) {
            indexData.push_back((GLuint) faces[i].mIndices[j]);
        }
    }
    setIndices(indexData);
}

void Mesh::prepareBuffers() {
    glBindVertexArray(vao);
    if (!meshData.position.empty()) {
        calculateBoundingBox();
        glGenBuffers(1, &positionBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(struct Position) * meshData.position.size(), &meshData.position[0],
                     GL_STATIC_DRAW);
    }
    if (!meshData.dColor.empty()) {
        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(struct Color) * meshData.dColor.size(), &meshData.dColor[0],
                     GL_STATIC_DRAW);
    }
    if (!meshData.normal.empty()) {
        glGenBuffers(1, &normalBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(struct Normal) * meshData.normal.size(), &meshData.normal[0],
                     GL_STATIC_DRAW);
    }
    if (!meshData.textureCoordinate.empty()) {
        glGenBuffers(1, &textureCoordinateBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, textureCoordinateBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(struct TextureCoordinate) * meshData.textureCoordinate.size(),
                     &meshData.textureCoordinate[0], GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

ShaderProgram::block Mesh::getMaterialBlock() {
    float power = 10.0f;
    float blendIndex = textures[0]->getBlendIndex();
    ShaderProgram::block material;
    material["Blend"] = {(void *) &blendIndex, sizeof(float)};
    material["MDiffuse"] = {&getDiffuseColor()[0], 4 * sizeof(float)};
    material["MAmbient"] = {&getAmbientColor()[0], 4 * sizeof(float)};
    material["MSpecular"] = {&getSpecularColor()[0], 4 * sizeof(float)};
    material["Specular_exponent"] = {&power, sizeof(float)};
    return material;
}

void Mesh::draw() {
    GLObject::draw();
    glBindVertexArray(vao);
    // glGetAttribLocation
    if (!meshData.position.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
    }
    if (meshData.textureCoordinate.empty()) {
        if (!meshData.dColor.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(1);
        }
    } else {
        if (!meshData.normal.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(1);
        }
        if (!meshData.textureCoordinate.empty()) {
            if (!textures.empty()) {
                for (int i = 0; i < textures.size(); i++) {
                    glBindBuffer(GL_ARRAY_BUFFER, textureCoordinateBuffer);
                    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
                    glEnableVertexAttribArray(2);
                    glActiveTexture(GL_TEXTURE0 + 2 * i);
                    glBindTexture(GL_TEXTURE_2D, textures[i]->id());
                }
            } else {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

//    if (shader != nullptr)
//        shader->enable();
    drawElements();

    glBindVertexArray(0);

    if (!meshData.position.empty())
        glDisableVertexAttribArray(0);
    if (meshData.textureCoordinate.empty()) {
        if (!meshData.dColor.empty())
            glDisableVertexAttribArray(1);
    } else {
        if (!meshData.normal.empty())
            glDisableVertexAttribArray(1);
        if (!meshData.textureCoordinate.empty()) {
            glDisableVertexAttribArray(2);
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_BLEND);
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    for (auto &i : children)  {
        i->draw();
    }
}

void Mesh::drawElements() {
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
}

void Mesh::calculateBoundingBox() {
    for (unsigned int i = 0; i < meshData.position.size(); i++) {
        if (this->meshData.position[i].x < boundingBox.first.x)
            boundingBox.first.x = this->meshData.position[i].x;
        if (this->meshData.position[i].x > boundingBox.second.x)
            boundingBox.second.x = this->meshData.position[i].x;
        if (this->meshData.position[i].y < boundingBox.first.y)
            boundingBox.first.y = this->meshData.position[i].y;
        if (this->meshData.position[i].y > boundingBox.second.y)
            boundingBox.second.y = this->meshData.position[i].y;
        if (this->meshData.position[i].z < boundingBox.first.z)
            boundingBox.first.z = this->meshData.position[i].z;
        if (this->meshData.position[i].z > boundingBox.second.z)
            boundingBox.second.z = this->meshData.position[i].z;
    }
}

Mesh::BBox Mesh::getBBox() {
    return boundingBox;
}

Mesh::~Mesh() {
    if (!meshData.position.empty())
        glDeleteBuffers(1, &positionBuffer);
    if (!meshData.dColor.empty())
        glDeleteBuffers(1, &colorBuffer);
    if (!meshData.normal.empty())
        glDeleteBuffers(1, &normalBuffer);
    if (!meshData.textureCoordinate.empty())
        glDeleteBuffers(1, &textureCoordinateBuffer);
    if (!indices.empty())
        glDeleteBuffers(1, &indexBuffer);
    for (auto &i: textures)
        delete i;
}