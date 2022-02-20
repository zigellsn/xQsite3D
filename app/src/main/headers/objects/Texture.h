#ifndef XQSITE_TEXTURE_H
#define XQSITE_TEXTURE_H

#include "xQsite3D.h"
#include <cassert>

class Texture {
public:
    Texture() {
        glGenTextures(1, &mId);
    };

    virtual ~Texture() {
        if (mId)
            glDeleteTextures(1, &mId);
    };

    GLuint id() const {
        return mId;
    };

    virtual GLuint target() const = 0;

    virtual int width() const = 0;

    virtual int height() const = 0;

    virtual float getBlendIndex() const = 0;

protected:
    GLuint mId{};
};

Texture *textureFromSurface(SDL_Surface *surface);

Texture *cubeMapFromSurfaces(std::vector<SDL_Surface *> surfaces);

Texture *cubeMapFromSurface(SDL_Surface *surface);

class GLTexture : public Texture {
public:
    GLTexture(int width, int height, unsigned int target);

    GLuint target() const override;

    int width() const override;

    int height() const override;

    float getBlendIndex() const override;

protected:
    unsigned int mTarget{};
    float mBlendIndex{};
    int mWidth{};
    int mHeight{};
};

class SimpleTexture : public GLTexture {
public:
    SimpleTexture(int width, int height);

    GLuint target() const override { return mTarget; }

    int storedWidth() const { return mStoredWidth; }

    int storedHeight() const { return mStoredHeight; }

    float getBlendIndex() const override { return mBlendIndex; }

    void initFromSurface(SDL_Surface *surface);

private:
    int mStoredWidth;
    int mStoredHeight;
};

class CubeMap : public GLTexture {
public:
    explicit CubeMap(int width = 0, int height = 0);

    void loadCubeMap(std::vector<SDL_Surface *> faces);

    void loadCubeMap(SDL_Surface *surface);

    GLuint target() const override { return GL_TEXTURE_CUBE_MAP; }
};

#endif //XQSITE_TEXTURE_H
