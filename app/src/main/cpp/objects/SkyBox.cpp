//
// Created by Simon on 17.02.2022.
//

#include "objects/SkyBox.h"

SkyBox::SkyBox(const std::vector<std::string> &faces) {
    string dir = "res/textures/";
    generateFaces();
    std::vector<SDL_Surface *> surfaces;
    for (const auto &path: faces) {
        string fullPath = dir + path;
        SDL_Surface *surface = IMG_Load(fullPath.c_str());
        surfaces.push_back(surface);
    }
    textures.push_back(cubeMapFromSurfaces(surfaces));
    for (auto &s: surfaces)
        SDL_FreeSurface(s);
}

SkyBox::SkyBox(const std::string &face) {
    string dir = "res/textures/";
    generateFaces();

    string fullPath = dir + face;
    SDL_Surface *surface = IMG_Load(fullPath.c_str());

    textures.push_back(cubeMapFromSurface(surface));
    SDL_FreeSurface(surface);
}

void SkyBox::generateFaces() {
    Mesh::Vertex data = {{
                                 {-1.0f, 1.0f, -1.0f, 1.0f},
                                 {-1.0f, -1.0f, -1.0f, 1.0f},
                                 {1.0f, -1.0f, -1.0f, 1.0f},
                                 {1.0f, -1.0f, -1.0f, 1.0f},
                                 {1.0f, 1.0f, -1.0f, 1.0f},
                                 {-1.0f, 1.0f, -1.0f, 1.0f},

                                 {-1.0f, -1.0f, 1.0f, 1.0f},
                                 {-1.0f, -1.0f, -1.0f, 1.0f},
                                 {-1.0f, 1.0f, -1.0f, 1.0f},
                                 {-1.0f, 1.0f, -1.0f, 1.0f},
                                 {-1.0f, 1.0f, 1.0f, 1.0f},
                                 {-1.0f, -1.0f, 1.0f, 1.0f},

                                 {1.0f, -1.0f, -1.0f, 1.0f},
                                 {1.0f, -1.0f, 1.0f, 1.0f},
                                 {1.0f, 1.0f, 1.0f, 1.0f},
                                 {1.0f, 1.0f, 1.0f, 1.0f},
                                 {1.0f, 1.0f, -1.0f, 1.0f},
                                 {1.0f, -1.0f, -1.0f, 1.0f},

                                 {-1.0f, -1.0f, 1.0f, 1.0f},
                                 {-1.0f, 1.0f, 1.0f, 1.0f},
                                 {1.0f, 1.0f, 1.0f, 1.0f},
                                 {1.0f, 1.0f, 1.0f, 1.0f},
                                 {1.0f, -1.0f, 1.0f, 1.0f},
                                 {-1.0f, -1.0f, 1.0f, 1.0f},

                                 {-1.0f, 1.0f, -1.0f, 1.0f},
                                 {1.0f, 1.0f, -1.0f, 1.0f},
                                 {1.0f, 1.0f, 1.0f, 1.0f},
                                 {1.0f, 1.0f, 1.0f, 1.0f},
                                 {-1.0f, 1.0f, 1.0f, 1.0f},
                                 {-1.0f, 1.0f, -1.0f, 1.0f},

                                 {-1.0f, -1.0f, -1.0f, 1.0f},
                                 {-1.0f, -1.0f, 1.0f, 1.0f},
                                 {1.0f, -1.0f, -1.0f, 1.0f},
                                 {1.0f, -1.0f, -1.0f, 1.0f},
                                 {-1.0f, -1.0f, 1.0f, 1.0f},
                                 {1.0f, -1.0f, 1.0f, 1.0f}
                         }};
    setVertexData(data);
    setIndices(generateDefaultIndices(data));
}

void SkyBox::drawElements() {
    glDepthFunc(GL_LEQUAL);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    glDepthFunc(GL_LESS);
}

void SkyBox::setViewMatrix(glm::mat4 matrix) {
    viewMatrix = glm::mat4(glm::mat3(matrix));
}
