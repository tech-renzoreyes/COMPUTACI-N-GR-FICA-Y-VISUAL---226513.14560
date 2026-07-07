#pragma once

#include <GL/glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
private:
    std::string vertexCode;
    std::string fragmentCode;

    unsigned int vertex;
    unsigned int fragment;

    const char* vertexShaderData;
    const char* fragmentShaderData;

    void checkCompileErrors(unsigned int shader, std::string type);

public:
    unsigned int ID;

    // Constructor
    Shader(const char* vertexPath, const char* fragmentPath);

    // usar shader
    void use();

    // uniforms
    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);

    void setVec3(const std::string& name, float x, float y, float z);
    void setVec3(const std::string& name, const glm::vec3& value);

    void setMat4(const std::string& name, const glm::mat4& value);
};