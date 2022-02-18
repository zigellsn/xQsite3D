#ifndef XQSITE_TEXTURE_H
#define XQSITE_TEXTURE_H

#include "xQsite3D.h"
#include <cassert>

class Texture;

Texture *textureFromSurface(SDL_Surface *surface);

Texture *cubeMapFromSurfaces(std::vector<SDL_Surface *> surfaces);

Texture *cubeMapFromSurface(SDL_Surface * surface);

class Texture {
public:
    virtual ~Texture() = default;

    virtual GLuint id() const = 0;

    virtual GLuint target() const = 0;

    virtual int width() const = 0;

    virtual int height() const = 0;

    virtual int stored_width() const = 0;

    virtual int stored_height() const = 0;

    virtual int offset_x() const = 0;

    virtual int offset_y() const = 0;

    virtual const Texture *parent() const = 0;

    virtual float getBlendIndex() const = 0;

    Texture *slice(int x, int y, int width, int height);
};

class SimpleTexture : public Texture {
public:
    SimpleTexture(int width, int height);

    ~SimpleTexture() override;

    GLuint id() const override { return m_id; }

    GLuint target() const override { return GL_TEXTURE_2D; }

    int width() const override { return m_width; }

    int height() const override { return m_height; }

    int stored_width() const override { return m_stored_width; }

    int stored_height() const override { return m_stored_height; }

    int offset_x() const override { return 0; }

    int offset_y() const override { return 0; }

    const Texture *parent() const override { return nullptr; }

    float getBlendIndex() const override { return blendIndex; }

    void initFromSurface(SDL_Surface *surface);

protected:
    GLuint m_id;

private:
    int m_width;
    int m_height;
    int m_stored_width;
    int m_stored_height;
    float blendIndex;
};

class CubeMap : public SimpleTexture {
public:
    explicit CubeMap(int width = 0, int height = 0);

    void loadCubeMap(std::vector<SDL_Surface *> faces);

    void loadCubeMap(SDL_Surface *surface);

    GLuint target() const override { return GL_TEXTURE_CUBE_MAP; }
};

class textureSlice : public Texture {
public:
    textureSlice(Texture *parent, int x, int y, int width, int height);

    GLuint id() const override { return m_parent->id(); }

    GLuint target() const override { return m_parent->target(); }

    int width() const override { return m_width; }

    int height() const override { return m_height; }

    int stored_width() const override { return m_parent->stored_width(); }

    int stored_height() const override { return m_parent->stored_height(); }

    int offset_x() const override { return m_offset_x; }

    int offset_y() const override { return m_offset_y; }

    const Texture *parent() const override { return m_parent; }

    float getBlendIndex() const override { return m_parent->getBlendIndex(); }

private:
    Texture *m_parent;
    int m_offset_x;
    int m_offset_y;
    int m_width;
    int m_height;
    float blendIndex;
};

#endif //XQSITE_TEXTURE_H
