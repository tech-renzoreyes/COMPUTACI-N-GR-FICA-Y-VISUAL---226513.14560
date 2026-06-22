
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include <iostream> 
#include "src/Shader.h" 

//  NAVE (2 TRIÁNGULOS) 
float vertices[] =
{
    // Punta superior
 0.0f,  0.48f, 0.0f,   0.0f,0.5f,1.0f,
-0.15f, 0.21f, 0.0f,   0.0f,0.5f,1.0f,
 0.15f, 0.21f, 0.0f,   0.0f,0.5f,1.0f,

 // Cuerpo inferior
 -0.15f, 0.21f, 0.0f,   0.0f,0.5f,1.0f,
  0.15f, 0.21f, 0.0f,   0.0f,0.5f,1.0f,
  0.0f, -0.45f, 0.0f,   0.0f,0.5f,1.0f,

  // Ala izquierda
  -0.15f, 0.15f, 0.0f,   0.0f,0.5f,1.0f,
  -0.54f,-0.03f, 0.0f,   0.0f,0.5f,1.0f,
   0.00f,-0.06f, 0.0f,   0.0f,0.5f,1.0f,

   // Ala derecha
    0.15f, 0.15f, 0.0f,   0.0f,0.5f,1.0f,
    0.54f,-0.03f, 0.0f,   0.0f,0.5f,1.0f,
    0.00f,-0.06f, 0.0f,   0.0f,0.5f,1.0f,
};




float posX = 0.0f;
float posY = 0.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool turbo = false;

void responsive(GLFWwindow* window, int width, int height);
void userInput(GLFWwindow* window);

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Nave Espacial", NULL, NULL);

    if (!window)
    {
        std::cout << "Error creando ventana\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, responsive);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error GLEW\n";
        return -1;
    }

    Shader myShader("res/Shader/vertexShader.glsl", "res/Shader/fragmentShader.glsl");

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Posición 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 *
        sizeof(float)));
    glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        userInput(window);

        myShader.use();

        // Movimiento 
        myShader.setFloat("xOffset", posX);
        myShader.setFloat("yOffset", posY);

        // Turbo 
        myShader.setFloat("turboActivo", turbo ? 1.0f : 0.0f);

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 12); //NUMERO DE VERTICES

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

void responsive(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void userInput(GLFWwindow* window)
{
    float cameraSpeed;

    //  TURBO 
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        cameraSpeed = 5.0f * deltaTime;
        turbo = true;
    }
    else
    {
        cameraSpeed = 2.5f * deltaTime;
        turbo = false;
    }

    // Movimiento 
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) posY += cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) posY -= cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) posX -= cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) posX += cameraSpeed;

    //  Efecto Pac-Man 
    if (posX > 1.0f) posX = -1.0f;
    if (posX < -1.0f) posX = 1.0f;

    if (posY > 1.0f) posY = -1.0f;
    if (posY < -1.0f) posY = 1.0f;
}