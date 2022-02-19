//
// Created by Simon on 18.02.2022.
//

#include "RenderPass.h"
#include "GL/glew.h"

RenderPass::RenderPass(int width, int height, int windowFbo) {
    this->windowFrameBuffer = windowFbo;
    Mesh::Vertex screen = {{{-1.0f, -1.0f, 0.0f, 1.0f}, {1.0f, -1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {-1.0f, 1.0f, 0.0f, 1.0f}},
                           {},
                           {},
                           {{0.0f,  0.0f},              {1.0f, 0.0f},              {1.0f, 1.0f},             {0.0f,  1.0f}}
    };

    setVertexData(screen);
    setIndices({0, 2, 3, 2, 0, 1});

    glGenFramebuffers(1, &frameBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    createTexture(width, height);
    createRenderBuffer(width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass::createRenderBuffer(int width, int height) {
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, windowFrameBuffer);
}

void RenderPass::createTexture(int width, int height) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderPass::apply(const std::function<void(RenderPass *)> &fp, bool active) {
    if (!active) {
        fp(this);
        return;
    }
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glBindTexture(GL_TEXTURE_2D, texture);
    fp(this);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, windowFrameBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RenderPass::draw(const std::function<void(GLObject *)> &fp) {
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, textureCoordinateBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    if (fp != nullptr)
        fp(this);
    drawElements();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glEnable(GL_DEPTH_TEST);
}

RenderPass::~RenderPass() {
    glDeleteFramebuffers(1, &frameBuffer);
    Mesh::~Mesh();
}
