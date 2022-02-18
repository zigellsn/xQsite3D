//
// Created by Simon on 07.02.2022.
//

#include "objects/AxisObject.h"


AxisObject::AxisObject() {
    Mesh::Vertex axis = {{{0.5f, 0.0f, 0.0f, 1.0f},
                                 {-0.5f, 0.0f, 0.0f, 1.0f},
                                 {0.5f, 0.0f, 0.0f, 1.0f},
                                 {0.49f, 0.01f, 0.0f, 1.0f},
                                 {0.5f, 0.0f, 0.0f, 1.0f},
                                 {0.49f, -0.01f, 0.0f, 1.0f},
                                 {0.0f, 0.5f, 0.0f, 1.0f},
                                 {0.0f, -0.5f, 0.0f, 1.0f},
                                 {0.0f, 0.5f, 0.0f, 1.0f},
                                 {0.01f, 0.49f, 0.0f, 1.0f},
                                 {0.0f, 0.5f, 0.0f, 1.0f},
                                 {-0.01f, 0.49f, 0.0f, 1.0f},
                                 {0.0f, 0.0f, 0.5f, 1.0f},
                                 {0.0f, 0.0f, -0.5f, 1.0f},
                                 {0.0f, 0.0f, 0.5f, 1.0f},
                                 {0.01f, 0.0f, 0.49f, 1.0f},
                                 {0.0f, 0.0f, 0.5f, 1.0f},
                                 {-0.01f, 0.0f, 0.49f, 1.0f}},
                         {{1.0f, 0.0f, 0.0f, 1.0f},
                                 {1.0f,  0.0f, 0.0f, 1.0f},
                                 {1.0f, 0.0f, 0.0f, 1.0f},
                                 {1.0f,  0.0f,  0.0f, 1.0f},
                                 {1.0f, 0.0f, 0.0f, 1.0f},
                                 {1.0f,  0.0f,   0.0f, 1.0f},
                                 {0.0f, 1.0f, 0.0f, 1.0f},
                                 {0.0f, 1.0f,  0.0f, 1.0f},
                                 {0.0f, 1.0f, 0.0f, 1.0f},
                                 {0.0f,  1.0f,  0.0f, 1.0f},
                                 {0.0f, 1.0f, 0.0f, 1.0f},
                                 {0.0f,   1.0f,  0.0f, 1.0f},
                                 {0.0f, 0.0f, 1.0f, 1.0f},
                                 {0.0f, 0.0f, 1.0f,  1.0f},
                                 {0.0f, 0.0f, 1.0f, 1.0f},
                                 {0.0f,  0.0f, 1.0f,  1.0f},
                                 {0.0f, 0.0f, 1.0f, 1.0f},
                                 {0.0f,   0.0f, 1.0f,  1.0f}}
    };
    setVertexData(axis);
    setIndices(generateDefaultIndices(axis));
}

void AxisObject::drawElements() {
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, nullptr);
}

