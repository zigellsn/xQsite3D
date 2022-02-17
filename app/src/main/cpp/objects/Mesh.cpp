#include "objects/Mesh.h"

#include <utility>

Mesh::Mesh(const std::string &name) : GLObject(name), indices(), positionBuffer(0), colorBuffer(0), normalBuffer(0),
                                      textureCoordinateBuffer(0),
                                      indexBuffer(0), vao(0), meshData() {
    glGenVertexArrays(1, &vao);
}

Mesh::Mesh(aiNode *node, aiMesh *mesh, const aiScene *scene) : Mesh() {
    string dir = "res/textures/";
    name = mesh->mName.data;
    aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
    fillMaterial(mat);

    unsigned int uvChannels = mesh->GetNumUVChannels();
    aiVector3D *textureCoordinates = nullptr;
    for (int j = 0; j < uvChannels; j++) {
        textureCoordinates = mesh->mTextureCoords[j];
        unsigned int textureCount = mat->GetTextureCount(aiTextureType_DIFFUSE);
        for (int i = 0; i < textureCount; i++) {
            aiString Path;
            if (mat->GetTexture(aiTextureType_DIFFUSE, i, &Path, nullptr,
                                nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS) {
                string path = Path.data;
                string base_filename = path.substr(path.find_last_of("/\\") + 1);
                string fullPath = dir + base_filename;
                SDL_Surface *surface = IMG_Load(fullPath.c_str());
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

    if (mesh->mAABB.mMin != aiVector3D(0.0f) || mesh->mAABB.mMax != aiVector3D(0.0f)) {
        boundingBox.first.x = mesh->mAABB.mMin.x;
        boundingBox.first.y = mesh->mAABB.mMin.y;
        boundingBox.first.z = mesh->mAABB.mMin.z;
        boundingBox.second.x = mesh->mAABB.mMax.x;
        boundingBox.second.y = mesh->mAABB.mMax.y;
        boundingBox.second.z = mesh->mAABB.mMax.z;
    } else {
        calculateBoundingBox();
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
    ShaderProgram::block materialBlock;
    materialBlock["Blend"] = {(void *) &blendIndex, sizeof(float)};
    materialBlock["MDiffuse"] = {&material.diffuse[0], 4 * sizeof(float)};
    materialBlock["MAmbient"] = {&material.ambient[0], 4 * sizeof(float)};
    materialBlock["MSpecular"] = {&material.specular[0], 4 * sizeof(float)};
    materialBlock["Specular_exponent"] = {&power, sizeof(float)};
    return materialBlock;
}

Mesh::Material Mesh::getMaterial() {
    return material;
}

void Mesh::draw(const std::function<void(GLObject *)> &fp) {
    GLObject::draw(fp);
    if (!meshData.position.empty()) {
        glDraw(fp);
    }
    if (fp != nullptr)
        fp(this);
    for (auto &i: children) {
        i.second->draw(fp);
    }
}

void Mesh::glDraw(const std::function<void(GLObject *)> &fp) {
    glBindVertexArray(vao);
    // glGetAttribLocation
    if (!meshData.position.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
    }
    if (!meshData.dColor.empty()) {
        if (meshData.textureCoordinate.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(1);
        }
    }

    if (!meshData.normal.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);
    }
    if (!textures.empty()) {
        for (int i = 0; i < textures.size(); i++) {
            if (!meshData.textureCoordinate.empty()) {
                glBindBuffer(GL_ARRAY_BUFFER, textureCoordinateBuffer);
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
                glEnableVertexAttribArray(2);
            }
            glActiveTexture(GL_TEXTURE0 + 2 * i);
            glBindTexture(textures[i]->target(), textures[i]->id());
        }
    } else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    if (fp != nullptr)
        fp(this);

    drawElements();

    glBindVertexArray(0);

    if (!meshData.position.empty())
        glDisableVertexAttribArray(0);
    if (!meshData.dColor.empty())
        if (meshData.textureCoordinate.empty()) {
            glDisableVertexAttribArray(1);
        }
    if (!meshData.normal.empty())
        glDisableVertexAttribArray(1);
    if (!textures.empty()) {
        glBindTexture(textures[0]->target(), 0);
        glDisable(GL_BLEND);
    }
    if (!meshData.textureCoordinate.empty()) {
        glDisableVertexAttribArray(2);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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

void Mesh::fillMaterial(aiMaterial *mat) {
    float blendIndex = 1.0f;

    if (!textures.empty())
        blendIndex = textures[0]->getBlendIndex();

    material = {
            blendIndex,
            getColor(mat, AI_MATKEY_COLOR_AMBIENT),
            getColor(mat, AI_MATKEY_COLOR_DIFFUSE),
            getColor(mat, AI_MATKEY_COLOR_SPECULAR),
            32.0f
    };
}

glm::vec4 Mesh::getColor(aiMaterial *mat, const char *key, unsigned int type,
                         unsigned int idx) {
    aiColor4D color;
    if (mat == nullptr) {
        return glm::vec4(1.0f);
    }
    if (mat->Get(key, type, idx, color) == AI_SUCCESS) {
        glm::vec4 col = glm::vec4(color.r, color.g, color.b, color.a);
        return col;
    }
    return glm::vec4(1.0f);
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

std::vector<GLuint> Mesh::generateDefaultIndices(const Mesh::Vertex &data) {
    std::vector<GLuint> ind;
    for (unsigned int i = 0; i < data.position.size(); i++) {
        ind.push_back(i);
    }
    return ind;
}
