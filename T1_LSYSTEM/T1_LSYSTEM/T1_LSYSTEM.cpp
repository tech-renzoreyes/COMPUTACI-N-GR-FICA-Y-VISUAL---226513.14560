#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <stack>
#include "src/Shader.h"

// Variables de cámara y tiempo
float posX = 0.0f;
float posY = 0.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// LCG para números aleatorios sin cstdlib
unsigned int my_seed = 54321;
int my_rand() {
    my_seed = my_seed * 1103515245 + 12345;
    return (my_seed / 65536) % 32768;
}

struct TurtleState {
    float x, y, angle;
    int depth;
};

// Propiedades de cada árbol y su arbusto
struct Tree {
    std::string sentence;
    float length;
    float angleDeg;
    float startAngle;
    float xOffset;
    float yOffset;
    float trunkR, trunkG, trunkB;
    float leafR, leafG, leafB;
    float windSpeed;
    float windSway;

    // Arbusto
    std::string bushSentence;
    float bushLength;
    float bushAngleDeg;
    float bushR, bushG, bushB;
    float bushXOffsetLocal;
};

void responsive(GLFWwindow* window, int width, int height);
void userInput(GLFWwindow* window);

// L-SYSTEM
std::string generateStochasticLSystem(std::string axiom, int iterations) {
    std::string current = axiom;

    for (int i = 0; i < iterations; i++) {
        std::string next = "";
        for (char c : current) {
            if (c == 'F') {
                int r = my_rand() % 3;
                if (r == 0) {
                    next += "FF+[+F-F+F]-[-F+F-F]";
                } else if (r == 1) {
                    next += "FF+[+F-F]-[-F+F]";
                } else {
                    next += "FF+[+F-F-F]-[-F+F+F]";
                }
            } else {
                next += c;
            }
        }
        current = next;
    }
    return current;
}

// VÉRTICES DEL ÁRBOL
std::vector<float> generateVertices(
    const std::string& sentence,
    float length,
    float angleDeg,
    float startAngle,
    float trunkR, float trunkG, float trunkB,
    float leafR, float leafG, float leafB,
    float windSpeed,
    float windSway
) {
    std::vector<float> vertices;
    std::stack<TurtleState> stack;

    float x = 0.0f;
    float y = 0.0f;
    float angle = startAngle;
    int depth = 0;
    float angleRad = angle * (3.14159265359f / 180.0f);

    float windEffect = std::sin(static_cast<float>(glfwGetTime()) * windSpeed);

    for (char c : sentence) {
        if (c == 'F') {
            float newX = x + length * std::cos(angleRad);
            float newY = y + length * std::sin(angleRad);

            float r, g, b;
            int thickness = 1;
            float move = 0.0f;

            // Tronco y ramas
            if (depth < 2.5) {
                r = trunkR;
                g = trunkG;
                b = trunkB;
                thickness = static_cast<int>(3.0f - depth);
            }
            // Hojas
            else {
                r = leafR;
                g = leafG;
                b = leafB;
                thickness = 0;
                move = windEffect * windSway * (depth - 1.5f);
            }

            for (int i = -thickness; i <= thickness; i++) {
                float offset = i * 0.002f;

                vertices.push_back(x + offset);
                vertices.push_back(y);
                vertices.push_back(0.0f);
                vertices.push_back(r);
                vertices.push_back(g);
                vertices.push_back(b);

                vertices.push_back(newX + offset + move);
                vertices.push_back(newY);
                vertices.push_back(0.0f);
                vertices.push_back(r);
                vertices.push_back(g);
                vertices.push_back(b);
            }

            x = newX;
            y = newY;
        }
        else if (c == '+') {
            angle -= angleDeg;
            angleRad = angle * (3.14159265359f / 180.0f);
        }
        else if (c == '-') {
            angle += angleDeg;
            angleRad = angle * (3.14159265359f / 180.0f);
        }
        else if (c == '[') {
            stack.push({ x, y, angle, depth });
            depth++;
        }
        else if (c == ']') {
            TurtleState state = stack.top();
            stack.pop();
            x = state.x;
            y = state.y;
            angle = state.angle;
            depth = state.depth;
            angleRad = angle * (3.14159265359f / 180.0f);
        }
    }
    return vertices;
}

// VÉRTICES DEL ARBUSTO
std::vector<float> generateBushVertices(
    const std::string& sentence,
    float length,
    float angleDeg,
    float startAngle,
    float r, float g, float b,
    float windTime,
    float windSwayMagnitude
) {
    std::vector<float> vertices;
    std::stack<TurtleState> stack;

    float x = 0.0f;
    float y = -0.1f;
    float angle = startAngle;
    int depth = 0;
    float angleRad = angle * (3.14159265359f / 180.0f);

    float windEffect = std::sin(windTime);

    for (char c : sentence) {
        if (c == 'F') {
            float newX = x + length * std::cos(angleRad);
            float newY = y + length * std::sin(angleRad);

            float move = windEffect * windSwayMagnitude * (depth + 1.0f);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(0.0f);
            vertices.push_back(r);
            vertices.push_back(g);
            vertices.push_back(b);

            vertices.push_back(newX + move);
            vertices.push_back(newY);
            vertices.push_back(0.0f);
            vertices.push_back(r);
            vertices.push_back(g);
            vertices.push_back(b);

            x = newX;
            y = newY;
        }
        else if (c == '+') {
            angle -= angleDeg;
            angleRad = angle * (3.14159265359f / 180.0f);
        }
        else if (c == '-') {
            angle += angleDeg;
            angleRad = angle * (3.14159265359f / 180.0f);
        }
        else if (c == '[') {
            stack.push({ x, y, angle, depth });
            depth++;
        }
        else if (c == ']') {
            TurtleState state = stack.top();
            stack.pop();
            x = state.x;
            y = state.y;
            angle = state.angle;
            depth = state.depth;
            angleRad = angle * (3.14159265359f / 180.0f);
        }
    }
    return vertices;
}

int main()
{
    // Inicializa OpenGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Crear ventana
    GLFWwindow* window = glfwCreateWindow(1200, 800, "T1 - LSYSTEM BOSQUE 2D", NULL, NULL);
    if (!window) { std::cout << "Error al crear ventana\n"; glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, responsive);

    if (glewInit() != GLEW_OK) { std::cout << "Error Glew\n"; glfwTerminate(); return -1; }

    Shader myShader("res/Shader/vertexShader.glsl", "res/Shader/fragmentShader.glsl");

    // Generar bosque
    std::vector<Tree> forest;

    for (int i = 0; i < 8; i++) {
        Tree t;
        float tFraction = static_cast<float>(i) / 7.0f;
        t.xOffset = -0.8f + tFraction * 1.6f + ((my_rand() % 100) / 100.0f - 0.5f) * 0.1f;

        float depthFactor = (my_rand() % 100) / 100.0f;
        t.yOffset = -0.7f - depthFactor * 0.15f;

        t.length = 0.009f + depthFactor * 0.007f;
        t.angleDeg = 18.0f + ((my_rand() % 100) / 100.0f) * 12.0f;
        t.startAngle = 90.0f;

        t.trunkR = 0.20f + ((my_rand() % 100) / 100.0f) * 0.08f;
        t.trunkG = 0.10f + ((my_rand() % 100) / 100.0f) * 0.04f;
        t.trunkB = 0.02f + ((my_rand() % 100) / 100.0f) * 0.02f;

        t.leafR = 0.02f + (1.0f - depthFactor) * 0.05f + ((my_rand() % 100) / 100.0f) * 0.04f;
        t.leafG = 0.45f + depthFactor * 0.30f + ((my_rand() % 100) / 100.0f) * 0.10f;
        t.leafB = 0.02f + (1.0f - depthFactor) * 0.08f;

        t.windSpeed = 1.6f + ((my_rand() % 100) / 100.0f) * 1.4f;
        t.windSway = 0.012f + ((my_rand() % 100) / 100.0f) * 0.015f;

        t.sentence = generateStochasticLSystem("F", 4);

        // Arbusto
        t.bushSentence = generateStochasticLSystem("F", 3);
        t.bushLength = t.length * 0.45f;
        t.bushAngleDeg = 20.0f + ((my_rand() % 100) / 100.0f) * 10.0f;
        t.bushXOffsetLocal = ((my_rand() % 100) / 100.0f - 0.5f) * 0.03f;

        t.bushR = 0.05f + ((my_rand() % 100) / 100.0f) * 0.10f;
        t.bushG = 0.35f + ((my_rand() % 100) / 100.0f) * 0.15f;
        t.bushB = 0.05f + ((my_rand() % 100) / 100.0f) * 0.05f;

        forest.push_back(t);
    }

    // Algoritmo del pintor (ordenar por profundidad)
    for (size_t i = 0; i < forest.size(); i++) {
        for (size_t j = i + 1; j < forest.size(); j++) {
            if (forest[i].yOffset < forest[j].yOffset) {
                Tree temp = forest[i];
                forest[i] = forest[j];
                forest[j] = temp;
            }
        }
    }

    // Buffers
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_PROGRAM_POINT_SIZE);

    // LOOP PRINCIPAL
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        userInput(window);

        myShader.use();

        // Celeste
        glClearColor(0.094f, 0.682f, 0.976f, 0.19f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);

        float globalWindTime = static_cast<float>(glfwGetTime()) * 2.2f;

        // Dibujar árboles y arbustos
        for (const auto& tree : forest) {
            
            // 1. Árbol
            std::vector<float> treeVertices = generateVertices(
                tree.sentence,
                tree.length,
                tree.angleDeg,
                tree.startAngle,
                tree.trunkR, tree.trunkG, tree.trunkB,
                tree.leafR, tree.leafG, tree.leafB,
                tree.windSpeed,
                tree.windSway
            );

            myShader.setFloat("xOffset", posX + tree.xOffset);
            myShader.setFloat("yOffset", posY + tree.yOffset);

            glBufferData(
                GL_ARRAY_BUFFER,
                treeVertices.size() * sizeof(float),
                treeVertices.data(),
                GL_DYNAMIC_DRAW
            );

            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(treeVertices.size() / 6));

            // 2. Arbusto
            std::vector<float> bushVertices = generateBushVertices(
                tree.bushSentence,
                tree.bushLength,
                tree.bushAngleDeg,
                90.0f,
                tree.bushR, tree.bushG, tree.bushB,
                globalWindTime,
                0.009f
            );

            myShader.setFloat("xOffset", posX + tree.xOffset + tree.bushXOffsetLocal);
            myShader.setFloat("yOffset", posY + tree.yOffset);

            glBufferData(
                GL_ARRAY_BUFFER,
                bushVertices.size() * sizeof(float),
                bushVertices.data(),
                GL_DYNAMIC_DRAW
            );

            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(bushVertices.size() / 6));
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void responsive(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void userInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    float cameraSpeed = 1.5f * deltaTime;

    // arriba
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        posY += cameraSpeed;

    // abajo
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        posY -= cameraSpeed;

    // izquierda
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        posX -= cameraSpeed;

    // derecha
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        posX += cameraSpeed;
}