//
// Created by Simon on 20.02.2022.
//

#include "render/ShadowPass.h"

ShadowPass::ShadowPass(int width, int height, int screenWidth, int screenHeight, int windowFbo) :
        RenderPass(width, height, windowFbo) {
    this->mScreenWidth = screenWidth;
    this->mScreenHeight = screenHeight;
}

void ShadowPass::createTexture(int width, int height) {
    glBindTexture(texture->target(), texture->id());
    glTexImage2D(texture->target(), 0, GL_DEPTH_COMPONENT,
                 width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(texture->target(), GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(texture->target(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(texture->target(), GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texture->target(), GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture->target(), texture->id(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindTexture(texture->target(), 0);
}

void ShadowPass::createRenderBuffer(int width, int height) {
}

void ShadowPass::apply(const function<void(RenderPass *)> &fp) {
    glViewport(0, 0, mWidth, mHeight);
    RenderPass::apply(fp);
    glViewport(0, 0, mScreenWidth, mScreenHeight);
}


