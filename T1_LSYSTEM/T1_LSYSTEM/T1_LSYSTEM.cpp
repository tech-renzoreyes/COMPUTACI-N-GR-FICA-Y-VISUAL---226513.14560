#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <iostream>

#include <cmath>

#include <vector>

#include <string>

#include <stack>

#include "src/Shader.h"



// Variables para las posiciones y tiempo

float posX = 0.0f;  // Movimiento horizontal de la escena

float posY = -0.8f; // Movimiento vertical

float deltaTime = 0.0f; // Tiempo entre frames

float lastFrame = 0.0f; // Último frame registrado



void responsive(GLFWwindow* window, int width, int height);

void userInput(GLFWwindow* window);


//L-SYSTEM arbol

std::string generateLSystem(std::string axiom, int iterations) {

    std::string current = axiom;

    for (int i = 0; i < iterations; i++) {

        std::string next = "";

        for (char c : current) {

            if (c == 'F') {

                // Regla principal del árbol fractal

                next += "FF+[+F-F+F]-[-F+F-F]"; //tipo de arbol

                //next += "FF+[+F-F]-[-F+F]";   //tipo de arbol


                //next += "FF+[+F-F-F]-[-F+F+F]";   //tipo de arbol

                 
            }

            else {

                next += c;

            }

        }
        current = next;
    }
    return current;

}


//Variables para las ramas

struct TurtleState {
    float x, y, angle;
    int depth;
};


// VÉRTICES DEL ÁRBOL

std::vector<float> generateVertices(
    const std::string& sentence,
    float length,
    float angleDeg,
    float startAngle
) {

    std::vector<float> vertices;
    std::stack<TurtleState> stack;


    float x = 0.0f;
    float y = 0.0f;

    // Dirección inicial del árbol
    float angle = startAngle;

    int depth = 0;

    float angleRad = angle * (3.14159265359f / 180.0f);


    for (char c : sentence) {

        if (c == 'F') {

            // dirección actual
            float newX = x + length * std::cos(angleRad);
            float newY = y + length * std::sin(angleRad);

            // Viento para hojas
            float windEffect = std::sin(glfwGetTime() * 2.0f);

            float r, g, b;
            float thickness = 1.0f;

            // TRONCO Y RAMAS
            // =========================
            if (depth < 2.5) //cantidad de tronco definido en 2.5
            {

    
                r = 0.25f; //color marron del tronco
                g = 0.12f;
                b = 0.03f;

                thickness = 3.0f - depth;
            }

         
            // HOJAS
            // =========================
            else {

                r = 0.1f;
                g = 0.8f;
                b = 0.1f;

                float move = windEffect * 0.02f;

                vertices.push_back(newX + move);
                vertices.push_back(newY);
                vertices.push_back(0.0f);

                vertices.push_back(r);
                vertices.push_back(g);
                vertices.push_back(b);

            }

            
            // GROSOR DE TRONCO Y RAMAS
            // =========================
            for (int i = -thickness; i <= thickness; i++) {


                float offset = i * 0.002f;


                // punto inicial

                vertices.push_back(x + offset);
                vertices.push_back(y);
                vertices.push_back(0.0f);

                vertices.push_back(r);
                vertices.push_back(g);
                vertices.push_back(b);



                // punto final

                vertices.push_back(newX + offset);
                vertices.push_back(newY);
                vertices.push_back(0.0f);

                vertices.push_back(r);
                vertices.push_back(g);
                vertices.push_back(b);

            }

            x = newX;
            y = newY;

        }

     
        // ROTAR DERECHA
        // =========================
        else if (c == '+') {

            angle -= angleDeg;

            angleRad =
                angle * (3.14159265359f / 180.0f);

        }

        
        // ROTAR IZQUIERDA
        // =========================
        else if (c == '-') {

            angle += angleDeg;

            angleRad =
                angle * (3.14159265359f / 180.0f);

        }

        // GUARDAR ESTADO
        // =========================
        else if (c == '[') {

            stack.push(
                { x, y, angle, depth }
            );
            depth++;
        }


        // RESTAURAR ESTADO
        // =========================
        else if (c == ']') {


            TurtleState state = stack.top();

            stack.pop();

            x = state.x;
            y = state.y;
            angle = state.angle;
            depth = state.depth;
            angleRad =
                angle * (3.14159265359f / 180.0f);

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
    GLFWwindow* window = glfwCreateWindow(1200, 800, "T1 - LSYSTEM", NULL, NULL);

    if (!window) { std::cout << "Error\n"; glfwTerminate(); return -1; }



    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, responsive);

    if (glewInit() != GLEW_OK) { std::cout << "Error Glew\n"; glfwTerminate(); return -1; }


    Shader myShader("res/Shader/vertexShader.glsl", "res/Shader/fragmentShader.glsl");


    //// Árbol grande (ramas hacia la derecha)
    //std::string treeBigInstructions = generateLSystem("F", 4);

    //std::vector<float> treeBig =
    //    generateVertices(
    //        treeBigInstructions,
    //        0.040f,
    //        22.0f,
    //        90.0f
    //    );


    //// Árbol mediano (ramas hacia la izquierda)
    //std::string treeMediumInstructions = generateLSystem("F", 4);

    //std::vector<float> treeMedium =
    //    generateVertices(
    //        treeMediumInstructions,
    //        0.038f,
    //        32.0f,
    //        270.0f
    //    );

   

    // Buffers
    unsigned int VBO, VAO;

    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);



    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
  
    // Posición

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);

    // Color

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(1);

    glEnable(GL_PROGRAM_POINT_SIZE);


    // LOOP PRINCIPAL

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        userInput(window);

        myShader.use();
        myShader.setFloat("xOffset", posX);
        myShader.setFloat("yOffset", posY);

        //glClearColor(0.60f, 0.85f, 1.00f, 1.0f); //azul claro
        glClearColor(0.01f, 0.03f, 0.12f, 1.0f); //azul oscuro
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);

        // Viento
        float wind = std::sin(glfwGetTime() * 1.5f) * 5.0f;



        // Árbol grande
        std::vector<float> treeBig =
            generateVertices(
                generateLSystem("F", 4),
                0.016f,
                20.0f,
                90.0f
               
            );


        // Árbol con ramas hacia la izquierda
        std::vector<float> treeMedium =
            generateVertices(
                generateLSystem("F", 4),
                0.014f,
                20.0f,
                90.0f
              
            );



        //// Árbol grande (normal)
        //std::vector<float> treeBig =
        //    generateVertices(
        //        generateLSystem("F", 4), //cantodad de ramas
        //        0.020f,
        //        20.0f,
        //        90.0f
        //    );


        //// Árbol con ramas al lado izquierdo
        //std::vector<float> treeMedium =
        //    generateVertices(
        //        generateLSystem("F", 4), //cantodad de ramas
        //        0.018f,
        //        -35.0f,
        //        90.0f
        //    );

        // ===== DIBUJAR ÁRBOL GRANDE =====

        myShader.setFloat("xOffset", posX - 0.15f);
        myShader.setFloat("yOffset", posY);

        glBufferData(
            GL_ARRAY_BUFFER,
            treeBig.size() * sizeof(float),
            treeBig.data(),
            GL_DYNAMIC_DRAW
        );

        glDrawArrays(GL_LINES, 0, treeBig.size() / 6);



        // ===== DIBUJAR ÁRBOL MEDIANO =====

        myShader.setFloat("xOffset", posX + 0.15f);
        myShader.setFloat("yOffset", posY);

        glBufferData(
            GL_ARRAY_BUFFER,
            treeMedium.size() * sizeof(float),
            treeMedium.data(),
            GL_DYNAMIC_DRAW
        );

        glDrawArrays(GL_LINES, 0, treeMedium.size() / 6);



        glBufferData(GL_ARRAY_BUFFER,
            treeMedium.size() * sizeof(float),
            treeMedium.data(),
            GL_DYNAMIC_DRAW);

        glDrawArrays(GL_LINES, 0, treeMedium.size() / 6);
        
        glBufferData(GL_ARRAY_BUFFER,
            treeMedium.size() * sizeof(float),
            treeMedium.data(),
            GL_DYNAMIC_DRAW);

        glDrawArrays(GL_LINES, 0, treeMedium.size() / 6);

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