//
// Created by Simon on 17.02.2022.
//

#include "objects/DebugObject.h"

glm::mat4 DebugObject::getTransformedModelMatrix(glm::mat4 modelMatrix, Mesh::BBox newBoundingBox) {
    glm::vec3 size = glm::vec3(newBoundingBox.second.x - newBoundingBox.first.x,
                               newBoundingBox.second.y - newBoundingBox.first.y,
                               newBoundingBox.second.z - newBoundingBox.first.z);
    glm::vec3 center = glm::vec3((newBoundingBox.first.x + newBoundingBox.second.x) / 2,
                                 (newBoundingBox.first.y + newBoundingBox.second.y) / 2,
                                 (newBoundingBox.first.z + newBoundingBox.second.z) / 2);
    glm::mat4 transform = glm::translate(glm::mat4(1), center)
                          * glm::scale(glm::mat4(1), size);

    return modelMatrix * transform;
}