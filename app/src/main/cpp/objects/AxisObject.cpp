//
// Created by Simon on 07.02.2022.
//

#include "objects/AxisObject.h"


AxisObject::AxisObject(float length) {
    setIndices({0, 1, 2, 3, 4, 5});
    setLength(length);
}

void AxisObject::drawElements() {
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, nullptr);
}

void AxisObject::setLength(float newLength) {
    this->length = newLength;
    Mesh::Vertex axis = {{{this->length, 0.0f, 0.0f, 1.0f},
                                 {-this->length, 0.0f, 0.0f, 1.0f},
                                 {0.0f, this->length, 0.0f, 1.0f},
                                 {0.0f, -this->length, 0.0f, 1.0f},
                                 {0.0f, 0.0f, this->length, 1.0f},
                                 {0.0f, 0.0f, -this->length, 1.0f}},
                         {{1.0f,         0.0f, 0.0f, 1.0f},
                                 {1.0f,          0.0f, 0.0f, 1.0f},
                                 {0.0f, 1.0f,         0.0f, 1.0f},
                                 {0.0f, 1.0f,          0.0f, 1.0f},
                                 {0.0f, 0.0f, 1.0f,         1.0f},
                                 {0.0f, 0.0f, 1.0f,          1.0f}}
    };
    setVertexData(axis);
}
