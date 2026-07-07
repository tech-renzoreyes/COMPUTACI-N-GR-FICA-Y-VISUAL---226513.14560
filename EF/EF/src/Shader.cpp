#include "Shader.h"

//-----------------------------------------------------
// CONSTRUCTOR
//-----------------------------------------------------
Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::ifstream vertexFile;
    std::ifstream fragmentFile;

    std::stringstream vertexStream;
    std::stringstream fragmentStream;

    vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vertexFile.open(vertexPath);
        fragmentFile.open(fragmentPath);

        vertexStream << vertexFile.rdbuf();
        fragmentStream << fragmentFile.rdbuf();

        vertexFile.close();
        fragmentFile.close();

        vertexCode = vertexStream.str();
        fragmentCode = fragmentStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR: NO SE PUDIERON LEER SHADERS\n";
        return;
    }

    vertexShaderData = vertexCode.c_str();
    fragmentShaderData = fragmentCode.c_str();

    //-----------------------------------------------------
    // VERTEX SHADER
    //-----------------------------------------------------
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderData, NULL);
    glCompileShader(vertex);

    int success;
    char infoLog[1024];

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
        std::cout << "ERROR VERTEX SHADER:\n" << infoLog << std::endl;
    }

    //-----------------------------------------------------
    // FRAGMENT SHADER
    //-----------------------------------------------------
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderData, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 1024, NULL, infoLog);
        std::cout << "ERROR FRAGMENT SHADER:\n" << infoLog << std::endl;
    }

    //-----------------------------------------------------
    // PROGRAM
    //-----------------------------------------------------
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 1024, NULL, infoLog);
        std::cout << "ERROR LINKING PROGRAM:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

//-----------------------------------------------------
// USE
//-----------------------------------------------------
void Shader::use()
{
    glUseProgram(ID);
}

//-----------------------------------------------------
// UNIFORMS
//-----------------------------------------------------
void Shader::setBool(const std::string& name, bool value)
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value)
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value)
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& value)
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}