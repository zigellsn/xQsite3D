#ifndef SDLTEST_MAINENGINE_H
#define SDLTEST_MAINENGINE_H

#include "xQsite3D.h"
#include "objects/Mesh.h"
#include "objects/Light.h"
#include "objects/Font.h"
#include "State.h"
#include "Input.h"
#include "objects/BBoxObject.h"
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

    void drawAxis();

    Uint32 flags;

    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;

    SDL_Window *win;
    SDL_GLContext mainGLContext;

    vector<Mesh *> meshes;
    vector<Camera *> cameras;
    vector<Light *> lights;

    State *state;
    Input *input;
    ShaderManager *shaderManager;
    Mesh *axisObject{};
    BBoxObject *bBoxObject{};
    Font *font{};

    ShaderProgram::block prepareMVPBlock(glm::mat4 modelMatrix);

    void initShaders();

    void drawBoundingBox(Mesh *mesh, glm::mat4 modelMatrix);
    void drawNormals(Mesh *mesh, glm::mat4 modelMatrix);
};

#endif //SDLTEST_MAINENGINE_H
