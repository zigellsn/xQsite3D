#include "objects/Font.h"

Font::Font(const std::string &fontname, int fontsize) : Mesh() {
    this->fontname = fontname;
    this->size = fontsize;
    modelMatrix = glm::mat4(1.0f);
}

void Font::renderText(const std::string &Text, float width, glm::vec4 color) {
    TTF_Font *ttffont = TTF_OpenFont(fontname.c_str(), size);
    SDL_Color fg;
    fg.r = (unsigned short) color[0] * 255;
    fg.g = (unsigned short) color[1] * 255;
    fg.b = (unsigned short) color[2] * 255;
    fg.a = (unsigned short) color[3] * 255;
    SDL_Surface *message = TTF_RenderUTF8_Blended(ttffont, Text.c_str(), fg);
    SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
    SDL_Surface *output = SDL_ConvertSurface(message, format, 0);
    SDL_FreeFormat(format);
    textures.push_back(textureFromSurface(output));
    SDL_FreeSurface(message);
    TTF_CloseFont(ttffont);

    float t_height = (float) textures[0]->height() / (float) textures[0]->width();
    float height = t_height * width;

    Mesh::Vertex data = {{{0.0f, 0.0f, 0.0f, 1.0f}, {width, 0.0f, 0.0f, 1.0f}, {width, -height, 0.0f, 1.0f}, {0.0f, -height, 0.0f, 1.0f}},
                         {},
                         {{0.0f, 0.0f, 1.0f, 1.0f}, {0.0f,  0.0f, 1.0f, 1.0f}, {0.0f,  0.0f,   1.0f, 1.0f}, {0.0f, 0.0f,   1.0f, 1.0f}},
                         {{0.0f, 0.0f},             {1.0f,  0.0f},             {1.0f,  1.0f},               {0.0f, 1.0f}}
    };

    setVertexData(data);
    setIndices({0, 3, 2, 2, 1, 0});
}

void Font::drawElements() {
    glClear(GL_DEPTH_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    glEnable(GL_DEPTH_BUFFER_BIT);
}
