#ifndef XQSITE_FONT_H
#define XQSITE_FONT_H

#include "xQsite3D.h"
#include "Mesh.h"

class Font : public Mesh {
public:
    Font(const std::string &fontname, int fontsize, glm::vec4 position = {0.0f, 0.0f, 0.0f, 0.0f});

    void renderText(const std::string &Text, glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    glm::vec2 getSize() const;

    glm::vec2 pos{};
    float height;

private:
    std::string fontname;
    int size;
    float width;
protected:
    void drawElements() override;

};

#endif //XQSITE_FONT_H
