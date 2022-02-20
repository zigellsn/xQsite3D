//
// Created by Simon on 18.02.2022.
//

#ifndef XQSITE3D_RENDERPASS_H
#define XQSITE3D_RENDERPASS_H


#include <functional>
#include "objects/Mesh.h"

class RenderPass : public Mesh {
public:
    RenderPass(int width, int height, int windowFbo);

    void init();

    virtual void apply(const std::function<void(RenderPass *)> &fp, bool active = true);

    virtual void draw(const std::function<void(GLObject *)> &fp, unsigned int externalTexture = 0);

    ~RenderPass() override;

    unsigned int texture{};

protected:
    virtual void createTexture(int width, int height);

    virtual void createRenderBuffer(int width, int height);

    unsigned int frameBuffer{};
    unsigned int renderBuffer = 0;

    int mWidth{};
    int mHeight{};

    int windowFrameBuffer;
};


#endif //XQSITE3D_RENDERPASS_H
