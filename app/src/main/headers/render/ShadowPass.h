//
// Created by Simon on 20.02.2022.
//

#ifndef XQSITE3D_SHADOWPASS_H
#define XQSITE3D_SHADOWPASS_H


#include "RenderPass.h"

class ShadowPass : public RenderPass {
public:
    ShadowPass(int width, int height, int screenWidth, int screenHeight, int windowFbo);

    void apply(const function<void(RenderPass *)> &fp, bool active = true) override;

protected:
    void createTexture(int width, int height) override;

    void createRenderBuffer(int width, int height) override;

private:
    int mScreenWidth{};
    int mScreenHeight{};

};


#endif //XQSITE3D_SHADOWPASS_H
