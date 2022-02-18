//
// Created by Simon on 17.02.2022.
//

#ifndef XQSITE3D_DEBUGOBJECT_H
#define XQSITE3D_DEBUGOBJECT_H


#include "Mesh.h"

class DebugObject : public Mesh {

public:
    virtual glm::mat4 getTransformedModelMatrix(glm::mat4 modelMatrix, GLObject::BBox newBoundingBox);

};


#endif //XQSITE3D_DEBUGOBJECT_H
