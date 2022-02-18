#ifndef SDLTEST_MAINENGINE_H
#define SDLTEST_MAINENGINE_H

#include "xQsite3D.h"
#include "objects/Mesh.h"
#include "objects/Light.h"
#include "objects/Font.h"
#include "State.h"
#include "Input.h"
#include "ShaderManager.h"

using namespace std;

class MainEngine {
public:
    MainEngine(int w, int h, bool fullscreen);

    virtual ~MainEngine();

    void MainLoop();

    void ImportScene(const string &filename);

private:

    void Init();

    void Update();

    void Draw();

    void calcFPS();

    void drawMainAxis();

    Uint32 flags;

    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;

    SDL_Window *win;
    SDL_GLContext mainGLContext;

    std::map<std::string, GLObject *> meshes;
    std::map<std::string, Camera *> cameras;
    std::map<std::string, Light *> lights;

    State *state;
    Input *input;
    ShaderManager *shaderManager;
    Mesh *axisObject{};
    Mesh *bBoxObject{};
    Mesh *skyBox{};
    Font *font{};

    ShaderProgram::block prepareMVPBlock(glm::mat4 modelMatrix, glm::mat4 normalMatrix = glm::mat4(1.0f));

    void initShaders();

    void drawBoundingBox(GLObject *mesh, glm::mat4 modelMatrix);

    void drawNormals(GLObject *mesh, glm::mat4 modelMatrix);

    void drawAxis(GLObject *mesh, glm::mat4 modelMatrix);

    void drawSkyBox();
};

#endif //SDLTEST_MAINENGINE_H
