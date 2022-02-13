#include "MainEngine.h"

#include <utility>
#include "xQsite3D.h"
#include "objects/Scene.h"
#include "objects/Mesh.h"
#include "objects/Font.h"
#include "objects/AxisObject.h"
#include "objects/BBoxObject.h"

MainEngine::MainEngine(int w, int h, bool fullscreen) {
    flags = SDL_WINDOW_OPENGL;
    if (fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    SCREEN_WIDTH = w;
    SCREEN_HEIGHT = h;

    win = nullptr;
    mainGLContext = nullptr;

    state = new State();
    input = new Input(state);

    shaderManager = new ShaderManager();
    Init();
}

void MainEngine::Init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        error(SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_SetRelativeMouseMode(SDL_TRUE);

    win = SDL_CreateWindow("OpenGL Test", SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, flags);
    if (win == nullptr) {
        error(SDL_GetError());
    }

    mainGLContext = SDL_GL_CreateContext(win);

    SDL_GL_MakeCurrent(win, mainGLContext);
    SDL_GL_SetSwapInterval(1);

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();
    if (status != GLEW_OK) {
        std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    const unsigned char *version = glGetString(GL_VERSION);
    if (version == nullptr) {
        error("An error occurred fetching GL_VERSION");
    } else {
        cout << "OpenGL version " << version << endl;
    }

    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    TTF_Init();

    axisObject = (Mesh *) new AxisObject(5.0f);
    bBoxObject = new BBoxObject();
    font = new Font("./res/font/ONESIZE_.TTF", 50);
    font->renderText("Text Test", {1.0f, 0.0f, 0.0f, 1.0f});
    font->factor = 0.5f;

    auto *camera = new Camera(glm::vec3(0.0f, 0.0f, -15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    camera->setPerspective(0.6911f, float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), 0.1f, 100.0f);
    cameras.push_back(camera);

    initShaders();
    state->state = GState::RUNNING;
}

void MainEngine::initShaders() {
    ShaderProgram *fontShader = shaderManager->createShader("font", "res/shader/text_vertex.glsl",
                                                            "res/shader/text_fragment.glsl");
    fontShader->appendAttribute("position");
    fontShader->appendAttribute("normal");
    fontShader->appendAttribute("texture");
    fontShader->link();

    ShaderProgram *shipShader = shaderManager->createShader("ship", "res/shader/ship_vertex.glsl",
                                                            "res/shader/ship_fragment.glsl");
    shipShader->appendAttribute("position");
    shipShader->appendAttribute("normal");
    shipShader->appendAttribute("texture");
    shipShader->link();

    ShaderProgram *axisShader = shaderManager->createShader("axis", "res/shader/axis_vertex.glsl",
                                                            "res/shader/axis_fragment.glsl");
    axisShader->appendAttribute("position");
    axisShader->appendAttribute("color");
    axisShader->link();
}

void MainEngine::MainLoop() {

    while (state->state == GState::RUNNING) {
        auto startTicks = (float) SDL_GetTicks64();

        input->ProcessInput(cameras[state->currentCamera]);
        Draw();
        SDL_GL_SwapWindow(win);
        calcFPS();
        static int frameCounter = 0;
        frameCounter++;
        if (frameCounter == 10) {
            Update();
            frameCounter = 0;
        }
        float frameTicks = (float) SDL_GetTicks64() - startTicks;
        if (1000.f / state->maxFPS > frameTicks) {
            SDL_Delay((unsigned int) (1000.f / state->maxFPS - frameTicks));
        }

    }
}

void MainEngine::Update() {
    std::ostringstream o;
    o << state->fps;
    string title = "OpenGL Test fps: " + o.str();
    SDL_SetWindowTitle(win, title.c_str());
}

void MainEngine::ImportScene(const string &filename) {
    auto *sl = new Scene();
    sl->loadFromFile(filename, BLENDER);

    meshes = sl->getMeshes();
    std::vector<Camera *> c = sl->getCameras();
    cameras.insert(cameras.end(), c.begin(), c.end());
    for (auto &cam: cameras) {
        cam->flipY = true;
        cam->flipX = true;
    }
    lights = sl->getLights();

    /*l = sl->getLight(1);
     if (l != nullptr) {
     l->setPosition(2.0f, 2.0f, -2.0f);
     lights.push_back(l);
     }*/

}

void MainEngine::Draw() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawAxis();
//    for (auto &light: lights) {
//        light->draw(shipShader, viewMatrix);
//    }

    for (unsigned int i = 0; i < meshes.size(); i++) {
        if (i == 0) {
            meshes[i]->rotate(Y_AXIS, 0.01f);
        }
        ShaderProgram::block matrices = prepareMVPBlock(meshes[i]->getModelMatrix());
        shaderManager->getShader("ship")->begin();
        glUniform1i(shaderManager->getShader("ship")->getUniformLocation("textureSampler"), 0);
        shaderManager->getShader("ship")->setUniformBlock("Matrices", matrices);
        meshes[i]->draw();
        shaderManager->getShader("ship")->end();

        if (state->debug) {
            bBoxObject->setBBox(meshes[i]->getBBox());
            matrices = prepareMVPBlock(bBoxObject->getTransformedModelMatrix(meshes[i]->getModelMatrix()));
            shaderManager->getShader("axis")->begin();
            shaderManager->getShader("axis")->setUniformBlock("Matrices", matrices);
            bBoxObject->draw();
            shaderManager->getShader("axis")->end();
        }
    }

    shaderManager->getShader("font")->begin();
    glUniform1f(shaderManager->getShader("font")->getUniformLocation("factor"), font->factor);
    glUniform2f(shaderManager->getShader("font")->getUniformLocation("pos"), -font->pos.x, font->pos.y);
    glUniform1i(shaderManager->getShader("font")->getUniformLocation("textureSampler"), 0);
    font->draw();
    shaderManager->getShader("font")->end();
}

ShaderProgram::block MainEngine::prepareMVPBlock(glm::mat4 modelMatrix) {
    glm::mat4 MVP =
            cameras[state->currentCamera]->projectionMatrix * cameras[state->currentCamera]->viewMatrix * modelMatrix;
    glm::mat4 N = glm::inverseTranspose(cameras[state->currentCamera]->viewMatrix * modelMatrix);
    ShaderProgram::block matrices;
    matrices["MVP"] = {(void *) &MVP[0][0], 16 * sizeof(float)};
    matrices["M"] = {(void *) &modelMatrix[0][0], 16 * sizeof(float)};
    matrices["V"] = {(void *) &cameras[state->currentCamera]->viewMatrix[0][0], 16 * sizeof(float)};
    matrices["P"] = {(void *) &cameras[state->currentCamera]->projectionMatrix[0][0], 16 * sizeof(float)};
    matrices["N"] = {(void *) &N[0][0], 16 * sizeof(float)};
    return matrices;
}

MainEngine::~MainEngine() {
    for (auto &mesh: meshes) {
        delete mesh;
    }

    SDL_GL_DeleteContext(mainGLContext);
    SDL_DestroyWindow(win);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void MainEngine::drawAxis() {
    shaderManager->getShader("axis")->begin();
    ShaderProgram::block matrices = prepareMVPBlock(glm::mat4(1.0f));
    shaderManager->getShader("axis")->setUniformBlock("Matrices", matrices);
    axisObject->draw();
    shaderManager->getShader("axis")->end();
}

void MainEngine::calcFPS() {
    static const int NUM_SAMPLES = 10;
    static float frameTimes[NUM_SAMPLES];

    static auto prevTicks = (float) SDL_GetTicks64();

    auto currentTicks = (float) SDL_GetTicks64();

    state->frameTime = currentTicks - prevTicks;
    frameTimes[state->currentFrame % NUM_SAMPLES] = state->frameTime;

    prevTicks = currentTicks;

    int count;

    state->currentFrame++;
    if (state->currentFrame < NUM_SAMPLES) {
        count = state->currentFrame;
    } else {
        count = NUM_SAMPLES;
    }

    float frameTimeAverage = 0.0f;
    for (int i = 0; i < count; i++) {
        frameTimeAverage += frameTimes[i];
    }
    frameTimeAverage /= (float) count;

    if (frameTimeAverage > 0.0f) {
        state->fps = 1000.0f / frameTimeAverage;
    } else {
        state->fps = 60.f;
    }
}

