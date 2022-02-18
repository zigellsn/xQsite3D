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

    void apply(const std::function<void(RenderPass *)> &fp, bool active = true);

    void draw(const std::function<void(GLObject *)> &fp) override;

    ~RenderPass() override;

private:
    unsigned int frameBuffer{};
    unsigned int texture{};
    unsigned int renderBuffer{};

    int windowFrameBuffer;
};


#endif //XQSITE3D_RENDERPASS_H
