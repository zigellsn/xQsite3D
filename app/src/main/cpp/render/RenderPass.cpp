//
// Created by Simon on 18.02.2022.
//

#include "render/RenderPass.h"
#include "GL/glew.h"

RenderPass::RenderPass(int width, int height, int windowFbo) {
    this->mWidth = width;
    this->mHeight = height;
    this->mWindowFrameBuffer = windowFbo;
    glGenFramebuffers(1, &mFrameBuffer);
    texture = new GLTexture(width, height, GL_TEXTURE_2D);
}

void RenderPass::init() {
    glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
    createTexture(this->mWidth, this->mHeight);
    createRenderBuffer(this->mWidth, this->mHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, mWindowFrameBuffer);
    mInitialized = true;
}

void RenderPass::createRenderBuffer(int width, int height) {
    glGenRenderbuffers(1, &mRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderBuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        error("Framebuffer not ready.");
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RenderPass::createTexture(int width, int height) {
    glBindTexture(texture->target(), texture->id());
    glTexImage2D(texture->target(), 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(texture->target(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture->target(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->target(), texture->id(), 0);
    glBindTexture(texture->target(), 0);
}

void RenderPass::apply(const std::function<void(RenderPass *)> &fp) {
    if (!mInitialized)
        return;
    if (!mActive) {
        fp(this);
        return;
    }
    if (mRenderBuffer)
        glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(texture->target(), texture->id());
    fp(this);
    glBindTexture(texture->target(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, mWindowFrameBuffer);
    if (mRenderBuffer)
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
}


RenderPass::~RenderPass() {
    glDeleteFramebuffers(1, &mFrameBuffer);
    if (mRenderBuffer)
        glDeleteRenderbuffers(1, &mRenderBuffer);
}

void RenderPass::activate() {
    mActive = true;
}

void RenderPass::deactivate() {
    mActive = false;
}
