//
// Created by Simon on 07.02.2022.
//

#ifndef XQSITE_AXISOBJECT_H
#define XQSITE_AXISOBJECT_H


#include "DebugObject.h"

class AxisObject : public DebugObject {
public:
    explicit AxisObject();

protected:
    void drawElements() override;

};


#endif //XQSITE_AXISOBJECT_H
