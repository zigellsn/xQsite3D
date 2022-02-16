#ifndef XQSITE_MESH_H
#define XQSITE_MESH_H

#include "GLObject.h"
#include "xQsite3D.h"

using namespace std;

class Mesh : public GLObject {
public:

    struct Position {
        float x{}, y{}, z{}, w = 1.0f;
    };

    struct Color {
        float r{}, g{}, b{}, a = 1.0f;
    };

    struct Normal {
        float x{}, y{}, z{}, w = 0.0f;
    };

    struct TextureCoordinate {
        float u{}, v{};
        // bool hasTexture = false;
    };

    struct Vertex {
        std::vector<Position> position; // Vertex
        std::vector<Color> dColor; // Diffuse color
        std::vector<Normal> normal; // Normal
        std::vector<TextureCoordinate> textureCoordinate; // Texcoord
    };

    typedef struct Material {
        float blendIndex;
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
        float power;
    } Material;

    typedef std::pair<glm::vec3, glm::vec3> BBox;

    explicit Mesh(aiNode *node, aiMesh *mesh, const aiScene *scene = nullptr);

    explicit Mesh(const std::string &name = "");

    ~Mesh() override;

    void setIndices(std::vector<GLuint> indexData);

    void setVertexData(Vertex vertex);

    BBox getBBox();

    ShaderProgram::block getMaterialBlock();

    Material getMaterial();

    void draw(const std::function<void(GLObject *)> &fp) override;

    std::map<std::string, Mesh *> children;

protected:

    void setIndices(unsigned int numFaces, const aiFace *faces);

    void drawElements() override;

    void prepareBuffers();

    std::vector<GLuint> indices;
    Vertex meshData;

    GLuint positionBuffer;
    GLuint colorBuffer;
    GLuint normalBuffer;
    GLuint textureCoordinateBuffer;
    GLuint indexBuffer;
    BBox boundingBox;

    Material material;

    GLuint vao;

    void calculateBoundingBox();

    glm::vec4 getColor(aiMaterial *mat, const char *key, unsigned int type, unsigned int idx);

    void fillMaterial(aiMaterial *mat);

private:
    void glDraw(const std::function<void(GLObject *)> &fp);
};

#endif //XQSITE_MESH_H
