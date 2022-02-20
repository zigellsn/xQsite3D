//
// Created by Simon on 20.02.2022.
//

#include "objects/ScreenObject.h"

ScreenObject::ScreenObject(Texture *texture) {
    Mesh::Vertex screen = {{{-1.0f, -1.0f, 0.0f, 1.0f}, {1.0f, -1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {-1.0f, 1.0f, 0.0f, 1.0f}},
                           {},
                           {},
                           {{0.0f,  0.0f},              {1.0f, 0.0f},              {1.0f, 1.0f},             {0.0f,  1.0f}}
    };

    setVertexData(screen);
    setIndices({0, 2, 3, 2, 0, 1});
    textures.push_back(texture);
}

void ScreenObject::draw(const function<void(GLObject *)> &fp) {
    assert(textures.size() == 1);
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, textureCoordinateBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(textures[0]->target(), textures[0]->id());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    if (fp != nullptr)
        fp(this);
    drawElements();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(textures[0]->target(), 0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glEnable(GL_DEPTH_TEST);
}

