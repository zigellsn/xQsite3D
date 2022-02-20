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

Texture *cubeMapFromSurface(SDL_Surface *surfaces) {
    auto *rv = new CubeMap(0, 0);
    rv->loadCubeMap(surfaces);
    return rv;
}

GLTexture::GLTexture(int width, int height, unsigned int target) {
    this->mWidth = width;
    this->mHeight = height;
    this->mTarget = target;
}

GLuint GLTexture::target() const {
    return this->mTarget;
}

int GLTexture::width() const {
    return this->mWidth;
}

int GLTexture::height() const {
    return this->mHeight;
}

float GLTexture::getBlendIndex() const {
    return 0.0f;
}

SimpleTexture::SimpleTexture(int width, int height) : GLTexture(width, height, GL_TEXTURE_2D) {
    mWidth = width;
    mHeight = height;
    mBlendIndex = 1.0f;

    if (SDL_GL_ExtensionSupported("GL_ARB_texture_non_power_of_two")) {
        mStoredWidth = mWidth;
        mStoredHeight = mHeight;
    } else {
        mStoredWidth = nextPowerOfTwo(mWidth);
        mStoredHeight = nextPowerOfTwo(mHeight);
    }
}

void SimpleTexture::initFromSurface(SDL_Surface *surface) {
    assert(surface->w == mWidth && surface->h == mHeight);
    glBindTexture(target(), mId);
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

    glTexImage2D(target(), 0, GL_RGBA8, mStoredWidth, mStoredHeight, 0,
                 format, GL_UNSIGNED_BYTE, surface->pixels);
    glGenerateMipmap(target());
    glBindTexture(target(), 0);
    mBlendIndex = 1.0f;
}

CubeMap::CubeMap(int width, int height) : GLTexture(width, height, GL_TEXTURE_2D) {
}

// Order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
void CubeMap::loadCubeMap(std::vector<SDL_Surface *> faces) {
    glBindTexture(target(), mId);
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

void CubeMap::loadCubeMap(SDL_Surface *surface) {
    int h = surface->h / 3;
    int w = surface->w / 4;
    std::vector<SDL_Surface *> faces;
    SDL_Surface *part = SDL_CreateRGBSurface(
            SDL_SWSURFACE,
            w, h,
            surface->format->BitsPerPixel,
            surface->format->Rmask,
            surface->format->Gmask,
            surface->format->Bmask,
            surface->format->Amask);
    SDL_Rect srcrect = {2 * w, h, w, h};
    SDL_BlitSurface(surface, &srcrect, part, nullptr);
    faces.push_back(part);

    part = SDL_CreateRGBSurface(
            SDL_SWSURFACE,
            w, h,
            surface->format->BitsPerPixel,
            surface->format->Rmask,
            surface->format->Gmask,
            surface->format->Bmask,
            surface->format->Amask);
    srcrect = {0, h, w, h};
    SDL_BlitSurface(surface, &srcrect, part, nullptr);
    faces.push_back(part);

    part = SDL_CreateRGBSurface(
            SDL_SWSURFACE,
            w, h,
            surface->format->BitsPerPixel,
            surface->format->Rmask,
            surface->format->Gmask,
            surface->format->Bmask,
            surface->format->Amask);
    srcrect = {w, 0, w, h};
    SDL_BlitSurface(surface, &srcrect, part, nullptr);
    faces.push_back(part);

    part = SDL_CreateRGBSurface(
            SDL_SWSURFACE,
            w, h,
            surface->format->BitsPerPixel,
            surface->format->Rmask,
            surface->format->Gmask,
            surface->format->Bmask,
            surface->format->Amask);
    srcrect = {w, 2 * h, w, h};
    SDL_BlitSurface(surface, &srcrect, part, nullptr);
    faces.push_back(part);

    part = SDL_CreateRGBSurface(
            SDL_SWSURFACE,
            w, h,
            surface->format->BitsPerPixel,
            surface->format->Rmask,
            surface->format->Gmask,
            surface->format->Bmask,
            surface->format->Amask);
    srcrect = {w, h, w, h};
    SDL_BlitSurface(surface, &srcrect, part, nullptr);
    faces.push_back(part);

    part = SDL_CreateRGBSurface(
            SDL_SWSURFACE,
            w, h,
            surface->format->BitsPerPixel,
            surface->format->Rmask,
            surface->format->Gmask,
            surface->format->Bmask,
            surface->format->Amask);
    srcrect = {3 * w, h, w, h};
    SDL_BlitSurface(surface, &srcrect, part, nullptr);
    faces.push_back(part);

    loadCubeMap(faces);
    for (auto &s: faces)
        SDL_FreeSurface(s);
}
