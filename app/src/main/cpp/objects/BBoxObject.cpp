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
                                {0.5,  -0.5, -0.5, 1.0},
                                {0.5,  0.5,  -0.5, 1.0},
                                {-0.5, 0.5,  -0.5, 1.0},
                                {-0.5, -0.5, 0.5,  1.0},
                                {0.5,  -0.5, 0.5,  1.0},
                                {0.5,  0.5,  0.5,  1.0},
                                {-0.5, 0.5,  0.5,  1.0}},
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

void BBoxObject::setBBox(Mesh::BBox newBoundingBox) {
    this->boundingBox = newBoundingBox;
}

glm::mat4 BBoxObject::getTransformedModelMatrix(glm::mat4 modelMatrix) const {
    glm::vec3 size = glm::vec3(boundingBox.second.x - boundingBox.first.x,
                               boundingBox.second.y - boundingBox.first.y,
                               boundingBox.second.z - boundingBox.first.z);
    glm::vec3 center = glm::vec3((boundingBox.first.x + boundingBox.second.x) / 2,
                                 (boundingBox.first.y + boundingBox.second.y) / 2,
                                 (boundingBox.first.z + boundingBox.second.z) / 2);
    glm::mat4 transform = glm::translate(glm::mat4(1), center)
                          * glm::scale(glm::mat4(1), size);

    return modelMatrix * transform;
}

void BBoxObject::drawElements() {
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, nullptr);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (GLvoid *) (4 * sizeof(GLuint)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (GLvoid *) (8 * sizeof(GLuint)));
}

