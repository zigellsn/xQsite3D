#include "objects/Texture.h"

#include <utility>

template<class T>
T nextPowerOfTwo(T value) {
    if ((value & (value - 1)) == 0)
        return value;
    value -= 1;
    for (size_t i = 1; i < sizeof(T) * 8; i <<= 1)
        value = value | value >> i;
    return value + 1;
}

Texture *textureFromSurface(SDL_Surface *surface) {
    if (surface == nullptr)
        return nullptr;
    auto *rv = new SimpleTexture(surface->w, surface->h);
    rv->initFromSurface(surface);
    return rv;
}

Texture *cubeMapFromSurfaces(std::vector<SDL_Surface *> surfaces) {
    auto *rv = new CubeMap(0, 0);
    rv->loadCubeMap(std::move(surfaces));
    return rv;
}

Texture *Texture::slice(int x, int y, int width, int height) {
    return new textureSlice(this, x, y, width, height);
}

SimpleTexture::SimpleTexture(int width, int height) {
    m_id = 0;
    m_width = width;
    m_height = height;
    blendIndex = 1.0f;

    if (SDL_GL_ExtensionSupported("GL_ARB_texture_non_power_of_two")) {
        m_stored_width = m_width;
        m_stored_height = m_height;
    } else {
        m_stored_width = nextPowerOfTwo(m_width);
        m_stored_height = nextPowerOfTwo(m_height);
    }
}

SimpleTexture::~SimpleTexture() {
    if (m_id)
        glDeleteTextures(1, &m_id);
}

void SimpleTexture::initFromSurface(SDL_Surface *surface) {
    assert(surface->w == m_width && surface->h == m_height);

    glGenTextures(1, &m_id);
    glBindTexture(target(), m_id);
    glTexParameteri(target(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(target(), GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(target(), GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(target(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLenum format;
    switch (surface->format->BytesPerPixel) {
        case 4:
            format = (surface->format->Rmask == 0x000000ff) ? GL_RGBA : GL_BGRA_EXT;
            break;
        case 3:
            format = (surface->format->Rmask == 0x000000ff) ? GL_RGB : GL_BGR_EXT;
            break;
        default:
            format = 0;
            error("Unknown texture format.");
    }

    glTexImage2D(target(), 0, GL_RGBA8, m_stored_width, m_stored_height, 0,
                 format, GL_UNSIGNED_BYTE, surface->pixels);
    glGenerateMipmap(target());
    glBindTexture(target(), 0);
    blendIndex = 1.0f;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
void CubeMap::loadCubeMap(std::vector<SDL_Surface *> faces) {
    glGenTextures(1, &m_id);
    glBindTexture(target(), m_id);

    for (unsigned int i = 0; i < faces.size(); i++) {
        GLenum format;
        switch (faces[i]->format->BytesPerPixel) {
            case 4:
                format = (faces[i]->format->Rmask == 0x000000ff) ? GL_RGBA : GL_BGRA_EXT;
                break;
            case 3:
                format = (faces[i]->format->Rmask == 0x000000ff) ? GL_RGB : GL_BGR_EXT;
                break;
            default:
                format = 0;
                error("Unknown texture format.");
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                     faces[i]->w, faces[i]->h, 0, format, GL_UNSIGNED_BYTE, faces[i]->pixels);
    }
    glTexParameteri(target(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(target(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(target(), GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(target(), 0);
}

CubeMap::CubeMap(int width, int height) : SimpleTexture(width, height) {

}

textureSlice::textureSlice(Texture *parent, int x, int y, int width,
                           int height) {
    blendIndex = 0.0f;
    m_parent = parent;
    m_offset_x = x;
    m_offset_y = y;
    m_width = width;
    m_height = height;
}
