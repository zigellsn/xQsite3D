//
// Created by Simon on 07.02.2022.
//

#ifndef XQSITE_AXISOBJECT_H
#define XQSITE_AXISOBJECT_H


#include "Mesh.h"

class AxisObject : public Mesh {
public:
    explicit AxisObject(float length = 1.0f);

    void setLength(float newLength = 1.0f);

protected:
    void drawElements() override;

private:
    float length = 1.0f;
};


#endif //XQSITE_AXISOBJECT_H
