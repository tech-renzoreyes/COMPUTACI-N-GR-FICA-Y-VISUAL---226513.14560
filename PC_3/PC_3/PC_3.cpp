#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include "src/Shader.h"

float vertices[] =
{
    // TRIANGULO 1 (Base izquierda)
    -0.8f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
    -0.4f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
    -0.6f,  0.0f, 0.0f,    0.0f, 1.0f, 0.0f,

    // TRIANGULO 2 (Base centro)
    -0.2f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
     0.2f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
     0.0f,  0.0f, 0.0f,    0.0f, 1.0f, 0.0f,

     // TRIANGULO 3 (Base derecha)
      0.4f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
      0.8f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
      0.6f,  0.0f, 0.0f,    0.0f, 1.0f, 0.0f,

      // TRIANGULO 4 (Superior independiente)
      -0.2f,  0.3f, 0.0f,    0.0f, 0.0f, 1.0f,
       0.2f,  0.3f, 0.0f,    1.0f, 0.0f, 0.0f,
       0.0f,  0.8f, 0.0f,    0.0f, 1.0f, 0.0f
};

int main()
{
    // Inicializar GLFW y configurar versión de OpenGL (3.3 Core)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Crear ventana
    GLFWwindow* window = glfwCreateWindow(1200, 800, "PC3 - G3", NULL, NULL);
    if (!window)
    {
        std::cout << "Error en la inicialización de la ventana\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Inicializar GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error en la inicialización de GLEW\n";
        glfwTerminate();
        return -1;
    }

    // Compilar e instanciar Shaders
    Shader myShader("res/Shader/vertexShader.glsl", "res/Shader/fragmentShader.glsl");

    // Configuración de Buffers (VAO, VBO)
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    // Atributo de Vértices (Location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Atributo de Colores (Location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Bucle de Renderizado Principal
    while (!glfwWindowShouldClose(window))
    {
        // Actualización de tiempos y cálculos matemáticos
        float time = glfwGetTime();
        float xValue = std::sin(time) / 2.0f + 0.5f;

        // Offsets para el movimiento circular
        float xOffset = cos(time * 2.0f) * 0.2f;
        float yOffset = -sin(time * 2.0f) * 0.2f;

        // Limpieza de pantalla
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Activar el shader y enviar Uniforms
        myShader.use();
        myShader.setFloat("xColor", xValue);
        myShader.setFloat("xOffset", xOffset);
        myShader.setFloat("yOffset", yOffset);

        // Dibujar los 4 triángulos (12 vértices en total)
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 12);

        // Intercambio de buffers y procesamiento de eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpieza de memoria
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}