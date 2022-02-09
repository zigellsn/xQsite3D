//
// Created by Simon on 07.02.2022.
//

#ifndef XQSITE_BBOXOBJECT_H
#define XQSITE_BBOXOBJECT_H


#include "Mesh.h"

class BBoxObject : public Mesh {
public:
    explicit BBoxObject(Mesh::Color color = {1.0f, 0.0f, 0.0f, 1.0f});

    void setBBox(BBox boundingBox);

    void setColor(Mesh::Color newColor = {1.0f, 0.0f, 0.0f, 1.0f});

    glm::mat4 getTransformedModelMatrix(glm::mat4 modelMatrix) const;

protected:
    void drawElements() override;

private:

    glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

};


#endif //XQSITE_BBOXOBJECT_H
