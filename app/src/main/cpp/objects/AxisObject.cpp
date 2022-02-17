//
// Created by Simon on 07.02.2022.
//

#include "objects/AxisObject.h"


AxisObject::AxisObject() {
    setIndices({0, 1, 2, 3, 4, 5});
    Mesh::Vertex axis = {{{0.5f, 0.0f, 0.0f, 1.0f},
                                 {-0.5f, 0.0f, 0.0f, 1.0f},
                                 {0.0f, 0.5f, 0.0f, 1.0f},
                                 {0.0f, -0.5f, 0.0f, 1.0f},
                                 {0.0f, 0.0f, 0.5f, 1.0f},
                                 {0.0f, 0.0f, -0.5f, 1.0f}},
                         {{1.0f, 0.0f, 0.0f, 1.0f},
                                 {1.0f,  0.0f, 0.0f, 1.0f},
                                 {0.0f, 1.0f, 0.0f, 1.0f},
                                 {0.0f, 1.0f,  0.0f, 1.0f},
                                 {0.0f, 0.0f, 1.0f, 1.0f},
                                 {0.0f, 0.0f, 1.0f,  1.0f}}
    };
    setVertexData(axis);
}

void AxisObject::drawElements() {
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, nullptr);
}

