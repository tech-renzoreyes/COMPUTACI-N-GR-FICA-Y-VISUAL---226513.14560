#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

#include "src/Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

// Dimensiones globales de la ventana
int windowWidth = 1200;
int windowHeight = 800;

// Variables de control de la cámara orbital
float cameraYaw = -90.0f;
float cameraPitch = 20.0f;
float cameraDistance = 35.0f;
float lastX = 600.0f;
float lastY = 400.0f;
bool firstMouse = true;

// Variables de control de la simulación
int focusedPlanetIndex = 0; // 0: Sol, 1: Mercurio, ..., 9: Plutón
float simSpeed = 1.0f;
float simTime = 0.0f;

// Estructura para almacenar las propiedades de los cuerpos celestes
struct Planet {
    GLuint textureID;
    float distance;      // Distancia orbital al Sol
    float orbitSpeed;    // Velocidad orbital (rad/s)
    float rotationSpeed; // Velocidad de rotación sobre su propio eje (rad/s)
    float scale;         // Tamaño / diámetro del planeta
    std::string name;    // Nombre para mostrar en consola
};

std::vector<Planet> planets;

// Declaraciones de funciones
void responsive(GLFWwindow* window, int width, int height);
void userInput(GLFWwindow* window);
void mouse_cursor_position(GLFWwindow* window, double xpos, double ypos);
void mouse_scroll_position(GLFWwindow* window, double xoffset, double yoffset);
unsigned int load_texture(const char* texture_path);
void generateSphere(float radius, unsigned int sectorCount, unsigned int stackCount, 
                    std::vector<float>& vertices, std::vector<unsigned int>& indices);
std::string resolvePath(const std::string& path);
void glfw_error_callback(int error, const char* description);

int main()
{
    // Registrar el callback de errores de GLFW antes de inicializar
    glfwSetErrorCallback(glfw_error_callback);

    // Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Error al inicializar GLFW\n";
        return -1;
    }

    // Configuración del contexto OpenGL (versión 3.3 Core Profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Crear ventana
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Simulador del Sistema Solar 3D - Computacion Grafica", NULL, NULL);
    if (!window)
    {
        std::cerr << "Error al crear la ventana GLFW\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    // Registrar los callbacks de interacción y respuesta a la ventana
    glfwSetFramebufferSizeCallback(window, responsive);
    glfwSetCursorPosCallback(window, mouse_cursor_position);
    glfwSetScrollCallback(window, mouse_scroll_position);

    // Inicializar GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Error al inicializar GLEW\n";
        glfwTerminate();
        return -1;
    }

    // Habilitar pruebas de profundidad y mezcla para transparencias
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --- Generación de la esfera base usando programación procedimental ---
    std::vector<float> sphereVertices;
    std::vector<unsigned int> sphereIndices;
    generateSphere(1.0f, 48, 24, sphereVertices, sphereIndices);

    // Configurar VAO, VBO y EBO para las esferas (planetas y sol)
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);

    // Atributos de los vértices de la esfera:
    // 1. Posición (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 2. Color (r, g, b)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 3. Coordenadas de textura (s, t)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // --- Preparación de geometría de órbitas circulares ---
    std::vector<float> orbitVertices;
    int orbitSegments = 120;
    for (int i = 0; i <= orbitSegments; ++i) {
        float theta = 2.0f * 3.14159265f * float(i) / float(orbitSegments);
        orbitVertices.push_back(cosf(theta)); // X
        orbitVertices.push_back(0.0f);        // Y
        orbitVertices.push_back(sinf(theta)); // Z
    }

    unsigned int orbitVAO, orbitVBO;
    glGenVertexArrays(1, &orbitVAO);
    glGenBuffers(1, &orbitVBO);
    glBindVertexArray(orbitVAO);
    glBindBuffer(GL_ARRAY_BUFFER, orbitVBO);
    glBufferData(GL_ARRAY_BUFFER, orbitVertices.size() * sizeof(float), orbitVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // --- Preparación de geometría de los anillos de Saturno ---
    std::vector<float> ringVertices;
    int ringSegments = 85;
    for (int i = 0; i <= ringSegments; ++i) {
        float theta = 2.0f * 3.14159265f * float(i) / float(ringSegments);
        float c = cosf(theta);
        float s = sinf(theta);
        
        // Vértice interno (radio relativo = 1.35)
        ringVertices.push_back(c * 1.35f);
        ringVertices.push_back(0.0f);
        ringVertices.push_back(s * 1.35f);
        ringVertices.push_back(0.0f); // U
        ringVertices.push_back(0.0f); // V
        
        // Vértice externo (radio relativo = 2.25)
        ringVertices.push_back(c * 2.25f);
        ringVertices.push_back(0.0f);
        ringVertices.push_back(s * 2.25f);
        ringVertices.push_back(1.0f); // U
        ringVertices.push_back(1.0f); // V
    }

    unsigned int ringVAO, ringVBO;
    glGenVertexArrays(1, &ringVAO);
    glGenBuffers(1, &ringVBO);
    glBindVertexArray(ringVAO);
    glBindBuffer(GL_ARRAY_BUFFER, ringVBO);
    glBufferData(GL_ARRAY_BUFFER, ringVertices.size() * sizeof(float), ringVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // --- Preparación de geometría del campo de estrellas (Starfield) ---
    srand(static_cast<unsigned int>(time(NULL)));
    std::vector<float> starVertices;
    int numStars = 1500;
    for (int i = 0; i < numStars; ++i) {
        float theta = ((float)rand() / RAND_MAX) * 2.0f * 3.14159265f;
        float phi = acosf(2.0f * ((float)rand() / RAND_MAX) - 1.0f);
        float radius = 80.0f + ((float)rand() / RAND_MAX) * 45.0f; // Distribuir estrellas en una esfera lejana
        
        float x = radius * sinf(phi) * cosf(theta);
        float y = radius * sinf(phi) * sinf(theta);
        float z = radius * cosf(phi);
        
        float b = 0.4f + ((float)rand() / RAND_MAX) * 0.6f; // Brillo aleatorio
        
        starVertices.push_back(x);
        starVertices.push_back(y);
        starVertices.push_back(z);
        starVertices.push_back(b); // R
        starVertices.push_back(b); // G
        starVertices.push_back(b); // B
    }

    unsigned int starVAO, starVBO;
    glGenVertexArrays(1, &starVAO);
    glGenBuffers(1, &starVBO);
    glBindVertexArray(starVAO);
    glBindBuffer(GL_ARRAY_BUFFER, starVBO);
    glBufferData(GL_ARRAY_BUFFER, starVertices.size() * sizeof(float), starVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // --- Carga de Shaders (Con resolución de rutas) ---
    std::string vertexShaderPath = resolvePath("res/Shader/vertexShader.glsl");
    std::string fragmentShaderPath = resolvePath("res/Shader/fragmentShader.glsl");
    Shader myShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
    myShader.use();
    myShader.setInt("planetTexture", 0);

    // --- Carga de Texturas (Usando rutas resueltas dinámicamente) ---
    GLuint textureSun     = load_texture(resolvePath("res/Texture/sun.jpg").c_str());
    GLuint textureMercury = load_texture(resolvePath("res/Texture/mercury.jpg").c_str());
    GLuint textureVenus   = load_texture(resolvePath("res/Texture/venus.jpg").c_str());
    GLuint textureEarth   = load_texture(resolvePath("res/Texture/tierra.jpg").c_str());
    GLuint textureMars    = load_texture(resolvePath("res/Texture/marte.jpg").c_str());
    GLuint textureJupiter = load_texture(resolvePath("res/Texture/jupiter.jpg").c_str());
    GLuint textureSaturn  = load_texture(resolvePath("res/Texture/saturno.jpg").c_str());
    GLuint textureUranus  = load_texture(resolvePath("res/Texture/urano.jpg").c_str());
    GLuint textureNeptune = load_texture(resolvePath("res/Texture/neptuno.jpg").c_str());
    GLuint texturePluto   = load_texture(resolvePath("res/Texture/pluton.jpg").c_str());

    // Registro de cuerpos celestes:
    // {TexturaID, distancia orbital, vel_órbita, vel_rotación, escala, nombre}
    planets = {
        { textureSun,      0.0f,  0.0f,   0.05f, 1.8f,  "Sol" },
        { textureMercury,  3.2f,  3.0f,   0.10f, 0.15f, "Mercurio" },
        { textureVenus,    4.8f,  2.2f,   0.08f, 0.25f, "Venus" },
        { textureEarth,    6.5f,  1.6f,   0.20f, 0.28f, "Tierra" },
        { textureMars,     8.2f,  1.2f,   0.18f, 0.20f, "Marte" },
        { textureJupiter, 11.0f,  0.7f,   0.40f, 0.65f, "Jupiter" },
        { textureSaturn,  14.5f,  0.5f,   0.35f, 0.55f, "Saturno" },
        { textureUranus,  18.0f,  0.3f,   0.25f, 0.40f, "Urano" },
        { textureNeptune, 21.0f,  0.2f,   0.22f, 0.38f, "Neptuno" },
        { texturePluto,   23.5f,  0.15f,  0.10f, 0.12f, "Pluton" }
    };

    // Imprimir el menú interactivo en la consola
    std::cout << "========================================================\n";
    std::cout << "          SIMULADOR INTERACTIVO DEL SISTEMA SOLAR       \n";
    std::cout << "========================================================\n";
    std::cout << "Controles de Visualizacion:\n";
    std::cout << "  - Arrastrar Clic Izquierdo : Rotar camara (Orbital)\n";
    std::cout << "  - Rueda del Mouse           : Zoom (Acercar/Alejar)\n";
    std::cout << "  - Teclas [0] - [9]          : Enfocar camara en un cuerpo:\n";
    std::cout << "      [0]: Sol      [1]: Mercurio  [2]: Venus    [3]: Tierra\n";
    std::cout << "      [4]: Marte    [5]: Jupiter   [6]: Saturno  [7]: Urano\n";
    std::cout << "      [8]: Neptuno  [9]: Pluton\n";
    std::cout << "Controles de Simulacion:\n";
    std::cout << "  - Flecha ARRIBA            : Aumentar velocidad del tiempo\n";
    std::cout << "  - Flecha ABAJO             : Disminuir velocidad del tiempo\n";
    std::cout << "  - Tecla [R]                 : Restablecer velocidad original (1.0x)\n";
    std::cout << "  - Tecla [ESC]               : Salir de la simulacion\n";
    std::cout << "========================================================\n";
    std::cout << "[Enfoque] Camara centrada en: Sol\n";

    float lastFrame = 0.0f;

    // --- Bucle de renderizado principal ---
    while (!glfwWindowShouldClose(window))
    {
        // Calcular delta de tiempo para independizar la velocidad de los frames por segundo (FPS)
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Actualizar el tiempo acumulado de la simulación basado en la velocidad actual
        simTime += deltaTime * simSpeed;

        // Procesar las entradas del teclado
        userInput(window);

        // Limpiar buffers de color y profundidad
        glClearColor(0.02f, 0.02f, 0.05f, 1.0f); // Espacio oscuro profundo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- Calcular la posición del objetivo enfocado ---
        glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
        if (focusedPlanetIndex > 0 && focusedPlanetIndex < (int)planets.size())
        {
            const auto& targetPlanet = planets[focusedPlanetIndex];
            glm::mat4 targetModel = glm::mat4(1.0f);
            float angleOrbit = simTime * targetPlanet.orbitSpeed;
            targetModel = glm::rotate(targetModel, angleOrbit, glm::vec3(0.0f, 1.0f, 0.0f));
            targetModel = glm::translate(targetModel, glm::vec3(targetPlanet.distance, 0.0f, 0.0f));
            // Extraer posición del planeta de la última columna de su matriz modelo
            cameraTarget = glm::vec3(targetModel[3]);
        }

        // --- Calcular matrices de Vista y Proyección ---
        float yawRad = glm::radians(cameraYaw);
        float pitchRad = glm::radians(cameraPitch);
        
        glm::vec3 cameraPos;
        cameraPos.x = cameraTarget.x + cameraDistance * cosf(pitchRad) * cosf(yawRad);
        cameraPos.y = cameraTarget.y + cameraDistance * sinf(pitchRad);
        cameraPos.z = cameraTarget.z + cameraDistance * cosf(pitchRad) * sinf(yawRad);

        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
        float aspect = (windowHeight > 0) ? (float)windowWidth / (float)windowHeight : 1.0f;
        if (aspect < 0.001f) aspect = 0.001f;
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 350.0f);

        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        // --- 1. Dibujar el campo de estrellas en el fondo (Skybox procedimental) ---
        glDepthMask(GL_FALSE); // Deshabilitar escritura en buffer de profundidad para dibujar de fondo
        myShader.setBool("isStarField", true);
        myShader.setBool("useTexture", false);
        myShader.setVec4("solidColor", -1.0f, -1.0f, -1.0f, 1.0f); // Sentinel para usar colores de los vértices
        glPointSize(2.0f); // Tamaño del punto de la estrella
        
        glBindVertexArray(starVAO);
        glDrawArrays(GL_POINTS, 0, numStars);
        
        glDepthMask(GL_TRUE); // Re-habilitar profundidad
        myShader.setBool("isStarField", false);

        // --- 2. Dibujar líneas de las órbitas orbitales de los planetas ---
        myShader.setBool("useTexture", false);
        myShader.setVec4("solidColor", 0.25f, 0.30f, 0.45f, 0.25f); // Color tenue para las lineas orbitales
        
        glBindVertexArray(orbitVAO);
        for (const auto& p : planets)
        {
            if (p.distance > 0.0f)
            {
                glm::mat4 orbitModel = glm::mat4(1.0f);
                orbitModel = glm::scale(orbitModel, glm::vec3(p.distance));
                myShader.setMat4("model", orbitModel);
                glDrawArrays(GL_LINE_STRIP, 0, orbitSegments + 1);
            }
        }

        // --- 3. Dibujar Sol y planetas del Sistema Solar ---
        myShader.setBool("useTexture", true);
        glActiveTexture(GL_TEXTURE0);

        for (size_t i = 0; i < planets.size(); ++i)
        {
            const auto& p = planets[i];
            glm::mat4 model = glm::mat4(1.0f);

            // Traslación (órbita alrededor del Sol si la distancia es mayor a 0)
            if (p.distance > 0.0f) {
                float angleOrbit = simTime * p.orbitSpeed;
                model = glm::rotate(model, angleOrbit, glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::translate(model, glm::vec3(p.distance, 0.0f, 0.0f));
            }

            // Almacenar matriz modelo del planeta antes de su rotación y escala propia (para los anillos)
            glm::mat4 planetBaseModel = model;

            // Rotación sobre su propio eje
            model = glm::rotate(model, simTime * p.rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
            
            // Escala del cuerpo celeste
            model = glm::scale(model, glm::vec3(p.scale));

            myShader.setMat4("model", model);
            myShader.setBool("isSun", (i == 0)); // El indice 0 es el Sol y emite luz propia

            // Vincular textura y dibujar
            glBindTexture(GL_TEXTURE_2D, p.textureID);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sphereIndices.size()), GL_UNSIGNED_INT, 0);

            // --- 4. Dibujar Anillos de Saturno ---
            if (i == 6) // Saturno es el índice 6
            {
                myShader.setBool("useTexture", false);
                // Color marrón-arena semitransparente para representar los anillos de roca y hielo
                myShader.setVec4("solidColor", 0.65f, 0.58f, 0.47f, 0.45f);

                // Aplicar la escala de Saturno pero manteniendo el plano
                glm::mat4 ringModel = glm::scale(planetBaseModel, glm::vec3(p.scale));
                
                // Inclinar ligeramente los anillos respecto a su plano orbital para mayor realismo (26 grados)
                ringModel = glm::rotate(ringModel, glm::radians(26.7f), glm::vec3(1.0f, 0.0f, 0.1f));
                myShader.setMat4("model", ringModel);

                glBindVertexArray(ringVAO);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, (ringSegments + 1) * 2);

                // Restaurar estado de textura para el siguiente planeta
                myShader.setBool("useTexture", true);
            }
        }

        // Intercambiar buffers y procesar eventos pendientes de GLFW
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Liberar recursos de buffers
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &orbitVAO);
    glDeleteBuffers(1, &orbitVBO);
    glDeleteVertexArrays(1, &ringVAO);
    glDeleteBuffers(1, &ringVBO);
    glDeleteVertexArrays(1, &starVAO);
    glDeleteBuffers(1, &starVBO);

    // Finalizar GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// Callback cuando se cambia el tamaño de la ventana
void responsive(GLFWwindow* window, int width, int height)
{
    if (width > 0 && height > 0)
    {
        windowWidth = width;
        windowHeight = height;
        glViewport(0, 0, width, height);
    }
}

// Controlar entrada del teclado para el enfoque y la velocidad de la simulación
void userInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    // Control de la velocidad de simulación (Flechas arriba y abajo)
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        simSpeed += 0.03f;
        if (simSpeed > 15.0f) simSpeed = 15.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        simSpeed -= 0.03f;
        if (simSpeed < -5.0f) simSpeed = -5.0f; // Permite retroceder el tiempo en órbita
    }
    // Restablecer la velocidad con la tecla 'R'
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        simSpeed = 1.0f;
    }

    // Cambiar enfoque de planeta mediante las teclas del teclado (0 al 9)
    int prevFocus = focusedPlanetIndex;
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) focusedPlanetIndex = 0;
    else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) focusedPlanetIndex = 1;
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) focusedPlanetIndex = 2;
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) focusedPlanetIndex = 3;
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) focusedPlanetIndex = 4;
    else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) focusedPlanetIndex = 5;
    else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) focusedPlanetIndex = 6;
    else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) focusedPlanetIndex = 7;
    else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) focusedPlanetIndex = 8;
    else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) focusedPlanetIndex = 9;

    // Si cambió el enfoque, lo mostramos en consola
    if (focusedPlanetIndex != prevFocus && focusedPlanetIndex >= 0 && focusedPlanetIndex < (int)planets.size()) {
        std::cout << "[Enfoque] Camara centrada en: " << planets[focusedPlanetIndex].name << "\n";
    }
}

// Callback de movimiento del ratón para rotar la cámara orbital (mientras se hace click izquierdo)
void mouse_cursor_position(GLFWwindow* window, double xpos, double ypos)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (firstMouse)
        {
            lastX = (float)xpos;
            lastY = (float)ypos;
            firstMouse = false;
        }

        float xoffset = (float)xpos - lastX;
        float yoffset = lastY - (float)ypos; // Invertido ya que las coordenadas Y van de abajo hacia arriba en pantalla

        lastX = (float)xpos;
        lastY = (float)ypos;

        float sensitivity = 0.25f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        cameraYaw += xoffset;
        cameraPitch += yoffset;

        // Limitar la rotación en el eje vertical (Pitch) para evitar volteretas
        if (cameraPitch > 89.0f)
            cameraPitch = 89.0f;
        if (cameraPitch < -89.0f)
            cameraPitch = -89.0f;
    }
    else
    {
        // Restablecer el flag al soltar el botón para evitar saltos bruscos al volver a clickear
        firstMouse = true;
    }
}

// Callback de scroll del ratón para realizar zoom (acercarse o alejarse)
void mouse_scroll_position(GLFWwindow* window, double xoffset, double yoffset)
{
    cameraDistance -= (float)yoffset * 1.5f;

    // Limites de zoom mínimo y máximo
    if (cameraDistance < 1.0f)
        cameraDistance = 1.0f;
    if (cameraDistance > 160.0f)
        cameraDistance = 160.0f;
}

// Función auxiliar para cargar texturas 2D con STB Image
unsigned int load_texture(const char* texture_path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Parámetros de envoltura y filtrado
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Voltear la textura verticalmente al cargarla para coincidir con el sistema UV de OpenGL
    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(texture_path, &width, &height, &nrChannels, 0);

    if (data)
    {
        // Detectar formato de la imagen según canales
        GLenum format = GL_RGB;
        if (nrChannels == 4)
            format = GL_RGBA;
        else if (nrChannels == 1)
            format = GL_RED;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Error al cargar la textura '" << texture_path << "': " << stbi_failure_reason() << std::endl;
        
        // Crear una textura blanca sólida de 1x1 píxeles como fallback de emergencia
        unsigned char whitePixel[4] = { 255, 255, 255, 255 };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data);
    return textureID;
}

// Genera una esfera UV procedimentalmente con posiciones, colores base y coordenadas UV
void generateSphere(float radius, unsigned int sectorCount, unsigned int stackCount, 
                    std::vector<float>& vertices, std::vector<unsigned int>& indices)
{
    const float PI = 3.14159265359f;
    float x, y, z, xy;
    float s, t;
    float sectorStep = 2.0f * PI / (float)sectorCount;
    float stackStep = PI / (float)stackCount;

    for (unsigned int i = 0; i <= stackCount; ++i)
    {
        float stackAngle = PI / 2.0f - (float)i * stackStep; // de pi/2 a -pi/2
        xy = radius * cosf(stackAngle);
        y = radius * sinf(stackAngle);

        for (unsigned int j = 0; j <= sectorCount; ++j)
        {
            float sectorAngle = (float)j * sectorStep;
            x = xy * cosf(sectorAngle);
            z = xy * sinf(sectorAngle);

            // Posición (X, Y, Z)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Color base (blanco para que se modifique por textura y luz en el fragment shader)
            vertices.push_back(1.0f);
            vertices.push_back(1.0f);
            vertices.push_back(1.0f);

            // Coordenadas UV (S, T)
            s = (float)j / (float)sectorCount;
            t = (float)i / (float)stackCount;
            vertices.push_back(s);
            vertices.push_back(t);
        }
    }

    // Generar índices
    for (unsigned int i = 0; i < stackCount; ++i)
    {
        unsigned int k1 = i * (sectorCount + 1);
        unsigned int k2 = k1 + sectorCount + 1;

        for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}

// Función auxiliar de errores de GLFW
void glfw_error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error (" << error << "): " << description << "\n";
}

// Función para resolver rutas locales de archivos (funciona tanto desde la carpeta de solución como del proyecto)
std::string resolvePath(const std::string& path)
{
    // Intentar abrir el archivo en la ruta directa
    std::ifstream f(path.c_str());
    if (f.good()) {
        return path;
    }
    
    // Si no se encuentra, intentar con el prefijo de la carpeta del proyecto "PC_6/"
    std::string alternativePath = "PC_6/" + path;
    std::ifstream f2(alternativePath.c_str());
    if (f2.good()) {
        return alternativePath;
    }
    
    // Retornar la ruta original como fallback para que el cargador correspondiente maneje el error
    return path;
}