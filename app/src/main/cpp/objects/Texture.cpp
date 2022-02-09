#include "objects/Texture.h"

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
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_stored_width, m_stored_height, 0,
                 format, GL_UNSIGNED_BYTE, surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    blendIndex = 1.0f;
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
