#include "MainEngine.h"

#include <utility>
#include "xQsite3D.h"
#include "objects/Scene.h"
#include "objects/Mesh.h"
#include "objects/Font.h"
#include "objects/AxisObject.h"
#include "objects/BBoxObject.h"
#include "objects/SkyBox.h"
#include "objects/ScreenObject.h"

MainEngine::MainEngine(int w, int h, bool fullscreen, const std::string &title) {
    flags = SDL_WINDOW_OPENGL;
    if (fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    SCREEN_WIDTH = w;
    SCREEN_HEIGHT = h;

    TITLE = title;

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

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_SetRelativeMouseMode(SDL_TRUE);

    win = SDL_CreateWindow(TITLE.c_str(), SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, flags);
    if (win == nullptr) {
        error(SDL_GetError());
    }

    mainGLContext = SDL_GL_CreateContext(win);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &window_fbo);
    SDL_GL_GetDrawableSize(win, &SCREEN_WIDTH, &SCREEN_HEIGHT);

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

    glEnable(GL_MULTISAMPLE);

    glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    const unsigned char *version = glGetString(GL_VERSION);
    if (version == nullptr) {
        error("An error occurred fetching GL_VERSION");
    } else {
        cout << "OpenGL version " << version << endl;
    }

    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    int mixerFlags = MIX_INIT_MOD | MIX_INIT_MP3;
    Mix_Init(mixerFlags);
    Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512);
    Mix_AllocateChannels(4);
    // sample = Mix_LoadMUS("./res/sound/inf.mp3");
    // Mix_PlayMusic(sample, -1);
    TTF_Init();

    axisObject = new AxisObject();
    bBoxObject = new BBoxObject();
    font = new Font("./res/font/ONESIZE_.TTF", 50);
    font->height = 0.05f;
    font->renderText("fps: 0.0", {1.0f, 0.0f, 0.0f, 1.0f});

    renderPass = new RenderPass(SCREEN_WIDTH, SCREEN_HEIGHT, window_fbo);
    renderPass->init();
    shadowPass = new ShadowPass(1024, 1024, SCREEN_WIDTH, SCREEN_HEIGHT, window_fbo);
    shadowPass->init();

    screenObject = new ScreenObject(renderPass->texture);

    initShaders();
    state->state = GState::RUNNING;
}

void MainEngine::initShaders() {
    ShaderProgram *shader = shaderManager->createShader("font", "res/shader/text_vertex.glsl",
                                                        "res/shader/text_fragment.glsl");
    shader->appendAttribute("position");
    shader->appendAttribute("normal");
    shader->appendAttribute("texture");
    shader->link();
    shader->apply([=](ShaderProgram *s) {
        glUniform1i(s->getUniformLocation("textureSampler"), 0);
    });


    shader = shaderManager->createShader("ship", "res/shader/ship_vertex.glsl",
                                         "res/shader/ship_fragment.glsl");
    shader->appendAttribute("position");
    shader->appendAttribute("normal");
    shader->appendAttribute("texture");
    shader->link();
    shader->apply([=](ShaderProgram *s) {
        glUniform1i(s->getUniformLocation("shadowSampler"), 1);
    });

    shader = shaderManager->createShader("normal", "res/shader/normal_vertex.glsl",
                                         "res/shader/normal_fragment.glsl",
                                         "res/shader/normal_geometry.glsl");
    shader->appendAttribute("position");
    shader->appendAttribute("normal");
    shader->link();

    shader = shaderManager->createShader("axis", "res/shader/axis_vertex.glsl",
                                         "res/shader/axis_fragment.glsl");
    shader->appendAttribute("position");
    shader->appendAttribute("color");
    shader->link();

    shader = shaderManager->createShader("skybox", "res/shader/skybox_vertex.glsl",
                                         "res/shader/skybox_fragment.glsl");
    shader->appendAttribute("position");
    shader->link();
    shader->apply([=](ShaderProgram *s) {
        glUniform1i(s->getUniformLocation("skybox"), 0);
    });

    shader = shaderManager->createShader("screen", "res/shader/screen_vertex.glsl",
                                         "res/shader/screen_fragment.glsl");
    shader->appendAttribute("position");
    shader->appendAttribute("texture");
    shader->link();
    shader->apply([=](ShaderProgram *s) {
        glUniform1i(s->getUniformLocation("textureSampler"), 0);
    });

    shader = shaderManager->createShader("shadow", "res/shader/shadow_vertex.glsl",
                                         "res/shader/shadow_fragment.glsl");
    shader->appendAttribute("position");
    shader->link();
}

void MainEngine::MainLoop() {

    while (state->state == GState::RUNNING) {
        auto startTicks = (float) SDL_GetTicks64();

        input->ProcessInput(cameras[state->currentCamera], [=](Uint32 key) {
            switch (key) {
                case SDL_SCANCODE_O:
                    state->currentCamera = "Camera";
                    break;
                case SDL_SCANCODE_I:
                    state->currentCamera = "mCamera";
                    break;
                case SDL_SCANCODE_L:
                    state->currentObject = meshes["Ship"];
                    break;
                case SDL_SCANCODE_K:
                    state->currentObject = cameras[state->currentCamera];
                    break;
                default:
                    break;
            }
        });
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
    string title = TITLE + " fps: " + o.str();
    font->renderText("fps: " + o.str(), {1.0f, 0.0f, 0.0f, 1.0f});
    SDL_SetWindowTitle(win, title.c_str());
}

void MainEngine::ImportScene(const string &filename) {
    auto *camera = new Camera(glm::vec3(0.0f, 0.0f, -15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    camera->setPerspective(0.6911f, float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), 0.1f, 100.0f);
    cameras["mCamera"] = camera;

    skyBox = new SkyBox({
                                "skybox/right.png",
                                "skybox/left.png",
                                "skybox/top.png",
                                "skybox/bottom.png",
                                "skybox/front.png",
                                "skybox/back.png"
                        });
//    skyBox = new SkyBox("skybox.png");
    auto *sl = new Scene();
    sl->loadFromFile(filename, BLENDER);

    meshes = sl->getMeshes();
    auto c = sl->getCameras();
    cameras.insert(c.begin(), c.end());
    for (auto &cam: cameras) {
        cam.second->flipY = true;
        cam.second->flipX = true;
        state->currentCamera = cam.first;
    }
    lights = sl->getLights();
    state->currentObject = meshes["Ship"];
}

void MainEngine::Draw() {
    shadowPass->apply([=](RenderPass *pass) {
        DrawShadowPass();
    });
    renderPass->apply([=](RenderPass *pass) {
        DrawObjects();
    });
    shaderManager->getShader("screen")->apply([=](ShaderProgram *s) {
        screenObject->draw(nullptr);
    });
}

void MainEngine::DrawObjects() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (state->debug)
        drawMainAxis();
    for (auto &mesh: meshes) {
        auto m = mesh.second;
        shaderManager->getShader("ship")->apply([=](ShaderProgram *s) {
            ShaderProgram::block matrices = prepareMVPBlock(m->getModelMatrix(), m->getNormalMatrix());
            glUniform3f(s->getUniformLocation("viewPos"),
                        cameras[state->currentCamera]->position.x,
                        cameras[state->currentCamera]->position.y,
                        cameras[state->currentCamera]->position.z);
            s->setUniformBlock("Matrices", matrices);
            m->draw([=](GLObject *mesh) {
                auto mat = ((Mesh *) mesh)->getMaterial();
                glUniform3f(s->getUniformLocation("material.ambient"), mat.ambient.x,
                            mat.ambient.y, mat.ambient.z);
                glUniform3f(s->getUniformLocation("material.diffuse"), mat.diffuse.x,
                            mat.diffuse.y, mat.diffuse.z);
                glUniform3f(s->getUniformLocation("material.specular"), mat.specular.x,
                            mat.specular.y, mat.specular.z);
                glUniform1f(s->getUniformLocation("material.shininess"), mat.power);
                if (mat.useDiffuseMap == GL_TRUE) {
                    glUniform1i(s->getUniformLocation("material.diffuseMap"), 0);
                    glUniform1i(s->getUniformLocation("material.useDiffuseMap"), GL_TRUE);
                } else {
                    glUniform1i(s->getUniformLocation("material.useDiffuseMap"), GL_FALSE);
                }
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, shadowPass->texture->id());
                int i = 0;
                for (auto &light: lights) {
                    auto l = light.second;
                    auto index = to_string(i);
                    glUniform3f(s->getUniformLocation("light[" + index + "].position"),
                                l->getPosition().x,
                                l->getPosition().y,
                                l->getPosition().z);
                    glUniform3f(s->getUniformLocation("light[" + index + "].ambient"), l->ambient.x, l->ambient.y,
                                l->ambient.z);
                    glUniform3f(s->getUniformLocation("light[" + index + "].diffuse"), l->diffuse.x, l->diffuse.y,
                                l->diffuse.z);
                    glUniform3f(s->getUniformLocation("light[" + index + "].specular"), l->specular.x, l->specular.y,
                                l->specular.z);
                    glUniform3f(s->getUniformLocation("light[" + index + "].direction"), l->direction.x, l->direction.y,
                                l->direction.z);
                    i++;
                }
            });
        });

        if (state->debug) {
            drawBoundingBox(m, m->getModelMatrix());
            drawNormals(m, m->getModelMatrix());
            drawAxis(m, m->getModelMatrix());
        }
    }
    if (state->debug) {
        for (auto &light: lights) {
            auto l = light.second;
            drawNormals(l, l->getModelMatrix());
            drawAxis(l, l->getModelMatrix());
        }
        for (auto &camera: cameras) {
            auto c = camera.second;
            drawNormals(c, c->getModelMatrix());
            drawAxis(c, c->getModelMatrix());
        }
    }

    drawSkyBox();
    shaderManager->getShader("font")->apply([=](ShaderProgram *s) {
        glUniform2f(s->getUniformLocation("pos"), -2.0f, 2.0f);
        font->draw(nullptr);
    });
}

void MainEngine::DrawShadowPass() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    float near_plane = 1.0f, far_plane = 7.5f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, cameras[state->currentCamera]->near,
                                           cameras[state->currentCamera]->far);
    glm::mat4 lightView = lights["Light_top"]->lookAt;
    lightSpaceMatrix = lightProjection * lightView;
    for (auto &mesh: meshes) {
        auto m = mesh.second;
        shaderManager->getShader("shadow")->apply([=](ShaderProgram *s) {
            m->draw([=](GLObject *mesh) {
                ShaderProgram::block matrices;
                matrices["M"] = {(void *) &mesh->getModelMatrix()[0][0], 16 * sizeof(float)};
                matrices["L"] = {(void *) &lightSpaceMatrix[0][0], 16 * sizeof(float)};
                s->setUniformBlock("Matrices", matrices);
            });
        });
    }
}

ShaderProgram::block MainEngine::prepareMVPBlock(glm::mat4 modelMatrix, glm::mat4 normalMatrix) {
    ShaderProgram::block matrices;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, cameras[state->currentCamera]->near,
                                           cameras[state->currentCamera]->far);
    glm::mat4 lightView = lights["Light_top"]->lookAt;
    lightSpaceMatrix = lightProjection * lightView;
    matrices["M"] = {(void *) &modelMatrix[0][0], 16 * sizeof(float)};
    matrices["V"] = {(void *) &cameras[state->currentCamera]->viewMatrix[0][0], 16 * sizeof(float)};
    matrices["P"] = {(void *) &cameras[state->currentCamera]->projectionMatrix[0][0], 16 * sizeof(float)};
    matrices["N"] = {(void *) &normalMatrix[0][0], 16 * sizeof(float)};
    matrices["L"] = {(void *) &lightSpaceMatrix[0][0], 16 * sizeof(float)};
    return matrices;
}

void MainEngine::drawMainAxis() {
    shaderManager->getShader("axis")->apply([=](ShaderProgram *s) {
        ShaderProgram::block matrices = prepareMVPBlock(
                ((AxisObject *) axisObject)->getTransformedModelMatrix(glm::mat4(1.0f),
                                                                       GLObject::BBox{{-5.0f, -5.0f, -5.0f},
                                                                                      {5.0f,  5.0f,  5.0f}}));
        s->setUniformBlock("Matrices", matrices);
        axisObject->draw(nullptr);
    });
}

void MainEngine::drawBoundingBox(GLObject *mesh, glm::mat4 modelMatrix) {
    shaderManager->getShader("axis")->apply([=](ShaderProgram *s) {
        ShaderProgram::block matrices = prepareMVPBlock(
                ((BBoxObject *) bBoxObject)->getTransformedModelMatrix(modelMatrix, mesh->getBBox()));
        s->setUniformBlock("Matrices", matrices);
        bBoxObject->draw(nullptr);
    });
    for (auto &m: mesh->children) {
        drawBoundingBox(m.second, modelMatrix);
    }
}

void MainEngine::drawNormals(GLObject *mesh, glm::mat4 modelMatrix) {
    shaderManager->getShader("normal")->apply([=](ShaderProgram *s) {
        ShaderProgram::block matrices = prepareMVPBlock(modelMatrix);
        s->setUniformBlock("Matrices", matrices);
        mesh->draw(nullptr);
    });
    for (auto &m: mesh->children) {
        drawNormals(m.second, modelMatrix);
    }
}

void MainEngine::drawAxis(GLObject *mesh, glm::mat4 modelMatrix) {
    shaderManager->getShader("axis")->apply([=](ShaderProgram *s) {
        GLObject::BBox bBox = mesh->getBBox();
        if (bBox == GLObject::emptyBBox) {
            bBox = {{0.5f,  0.5f,  0.5f},
                    {-0.5f, -0.5f, -0.5f}};
        } else {
            bBox.first = bBox.first * 1.5f;
            bBox.second = bBox.second * 1.5f;
        }
        ShaderProgram::block matrices = prepareMVPBlock(
                ((AxisObject *) axisObject)->getTransformedModelMatrix(modelMatrix, bBox));
        s->setUniformBlock("Matrices", matrices);
        axisObject->draw(nullptr);
    });
    for (auto &m: mesh->children) {
        drawAxis(m.second, modelMatrix);
    }
}

void MainEngine::drawSkyBox() {
    shaderManager->getShader("skybox")->apply([=](ShaderProgram *s) {
        ShaderProgram::block matrices;
        ((SkyBox *) skyBox)->setViewMatrix(cameras[state->currentCamera]->viewMatrix);
        matrices["V"] = {(void *) &((SkyBox *) skyBox)->viewMatrix[0][0], 16 * sizeof(float)};
        matrices["P"] = {(void *) &cameras[state->currentCamera]->projectionMatrix[0][0], 16 * sizeof(float)};
        s->setUniformBlock("Matrices", matrices);
        skyBox->draw(nullptr);
    });
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

MainEngine::~MainEngine() {
    for (auto &mesh: meshes) {
        delete mesh.second;
    }

    SDL_GL_DeleteContext(mainGLContext);
    SDL_DestroyWindow(win);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    // Mix_FreeMusic(sample);
    Mix_CloseAudio();
    Mix_Quit();
}
