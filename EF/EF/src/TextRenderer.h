#pragma once

#include <map>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct Character {
    unsigned int TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    unsigned int Advance;
};

class TextRenderer {
public:
    std::map<char, Character> Characters;
    unsigned int VAO, VBO;

    void Init();
    void LoadFont(const char* path);
    void RenderText(unsigned int shader, std::string text, float x, float y, float scale, glm::vec3 color);
};
