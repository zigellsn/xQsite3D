//
// Created by Simon on 07.02.2022.
//

#include "objects/BBoxObject.h"

BBoxObject::BBoxObject(Mesh::Color color) {
    setIndices({0, 1, 2, 3, 4, 5, 6, 7, 0, 4, 1, 5, 2, 6, 3, 7});
    setColor(color);
}

void BBoxObject::setColor(Mesh::Color newColor) {
    Mesh::Vertex box = {{{-0.5, -0.5, -0.5, 1.0},
                                {0.5, -0.5, -0.5, 1.0},
                                {0.5, 0.5, -0.5, 1.0},
                                {-0.5, 0.5, -0.5, 1.0},
                                {-0.5, -0.5, 0.5, 1.0},
                                {0.5, -0.5, 0.5, 1.0},
                                {0.5, 0.5, 0.5, 1.0},
                                {-0.5, 0.5, 0.5, 1.0}},
                        {newColor,
                                newColor,
                                newColor,
                                newColor,
                                newColor,
                                newColor,
                                newColor,
                                newColor}
    };
    setVertexData(box);
}

void BBoxObject::drawElements() {
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, nullptr);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (GLvoid *) (4 * sizeof(GLuint)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (GLvoid *) (8 * sizeof(GLuint)));
}

