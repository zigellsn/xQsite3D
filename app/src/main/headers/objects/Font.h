#ifndef XQSITE_FONT_H
#define XQSITE_FONT_H

#include "xQsite3D.h"
#include "Mesh.h"

class Font : public Mesh {
public:
    Font(const std::string &fontname, int fontsize);

    void renderText(const std::string &Text, float width = 1.0f, glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

private:
    std::string fontname;
    int size;
protected:
    void drawElements() override;

};

#endif //XQSITE_FONT_H
