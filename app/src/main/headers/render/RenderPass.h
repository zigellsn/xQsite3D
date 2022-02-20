//
// Created by Simon on 18.02.2022.
//

#ifndef XQSITE3D_RENDERPASS_H
#define XQSITE3D_RENDERPASS_H


#include <functional>
#include "objects/Mesh.h"

class RenderPass {
public:
    RenderPass(int width, int height, int windowFbo);

    void init();

    void activate();

    void deactivate();

    virtual void apply(const std::function<void(RenderPass *)> &fp);

    virtual ~RenderPass();

    Texture *texture;

protected:
    virtual void createTexture(int width, int height);

    virtual void createRenderBuffer(int width, int height);

    unsigned int mFrameBuffer{};
    unsigned int mRenderBuffer = 0;

    int mWidth{};
    int mHeight{};

    int mWindowFrameBuffer;

    bool mActive = true;

private:
    bool mInitialized = false;
};


#endif //XQSITE3D_RENDERPASS_H
