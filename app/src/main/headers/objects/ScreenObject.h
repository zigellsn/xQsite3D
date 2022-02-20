//
// Created by Simon on 20.02.2022.
//

#ifndef XQSITE3D_SCREENOBJECT_H
#define XQSITE3D_SCREENOBJECT_H

#include "Mesh.h"

class ScreenObject : public Mesh {
public:
    explicit ScreenObject(Texture *texture);

    void draw(const function<void(GLObject *)> &fp) override;

};


#endif //XQSITE3D_SCREENOBJECT_H
