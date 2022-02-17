//
// Created by Simon on 07.02.2022.
//

#ifndef XQSITE_BBOXOBJECT_H
#define XQSITE_BBOXOBJECT_H


#include "DebugObject.h"

class BBoxObject : public DebugObject {
public:
    explicit BBoxObject(Mesh::Color color = {1.0f, 0.0f, 0.0f, 1.0f});

    void setColor(Mesh::Color newColor = {1.0f, 0.0f, 0.0f, 1.0f});

protected:
    void drawElements() override;

};


#endif //XQSITE_BBOXOBJECT_H
