#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <ctime>
#include <algorithm> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include "src/Shader.h"
#include "TextRenderer.h"

using namespace std;

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

float deltaTime = 0;
float lastFrame = 0;

// Variables de Estado del Juego
int lives = 3;
int completedWords = 0;
bool isPaused = false;
bool isGameOver = false;
bool isLevelCleared = false;
int currentLevel = 1;
bool isGameStarted = false;

struct WordPair {
    string english;
    string spanish;
};

vector<WordPair> level1Words = {
    {"apple", "manzana"},
    {"dog", "perro"},
    {"cat", "gato"},
    {"sun", "sol"},
    {"book", "libro"}
};

vector<WordPair> level2Words = {
    {"house", "casa"},
    {"chair", "silla"},
    {"table", "mesa"},
    {"water", "agua"},
    {"school", "escuela"}
};

vector<WordPair> level3Words = {
    {"airplane", "avion"},
    {"computer", "computadora"},
    {"family", "familia"},
    {"teacher", "profesor"},
    {"student", "estudiante"}
};

vector<WordPair> levelWords = level1Words;
int currentWordIndex = 0;

struct GameObject {
    glm::vec3 position;
    bool isCircle; // true si es Sol, false si es Nube
    bool active;
};

vector<GameObject> activeObjects;

float spawnTimer = 0.0f;
const float SPAWN_INTERVAL = 1.5f; // Intervalo de los objetos exactamente cada 1.5s

// El avión inicia volando alto en el cielo celeste
glm::vec3 planePos(0.0f, 5.0f, 0.0f);
float speed = 12.0f;
TextRenderer text;

// --- FUNCIONES DE DIBUJO DE GEOMETRÍAS ---
void drawDetailedAirplane(glm::mat4 planeModel, Shader& shaderProgram) {
    // --- 1. FUSELAJE (Cuerpo Principal Volum�trico - Rojo) ---
    shaderProgram.setMat4("model", planeModel);
    shaderProgram.setVec3("color", glm::vec3(0.85f, 0.15f, 0.15f)); // Rojo deportivo
    
    glBegin(GL_TRIANGLES);
    // Punta a cabina (Volumen 3D)
    glVertex3f(-1.2f, 0.0f, 0.0f);
    glVertex3f(-0.3f, 0.35f, 0.35f);
    glVertex3f(-0.3f, 0.35f, -0.35f);

    glVertex3f(-1.2f, 0.0f, 0.0f);
    glVertex3f(-0.3f, -0.35f, -0.35f);
    glVertex3f(-0.3f, -0.35f, 0.35f);

    glVertex3f(-1.2f, 0.0f, 0.0f);
    glVertex3f(-0.3f, -0.35f, 0.35f);
    glVertex3f(-0.3f, 0.35f, 0.35f);

    glVertex3f(-1.2f, 0.0f, 0.0f);
    glVertex3f(-0.3f, 0.35f, -0.35f);
    glVertex3f(-0.3f, -0.35f, -0.35f);

    // Cuerpo medio a cola (Bloque 3D grueso)
    // Top
    glVertex3f(-0.3f, 0.35f, -0.35f); glVertex3f(-0.3f, 0.35f, 0.35f); glVertex3f(0.8f, 0.15f, 0.15f);
    glVertex3f(0.8f, 0.15f, 0.15f); glVertex3f(0.8f, 0.15f, -0.15f); glVertex3f(-0.3f, 0.35f, -0.35f);
    // Bottom
    glVertex3f(-0.3f, -0.35f, -0.35f); glVertex3f(0.8f, -0.15f, -0.15f); glVertex3f(-0.3f, -0.35f, 0.35f);
    glVertex3f(-0.3f, -0.35f, 0.35f); glVertex3f(0.8f, -0.15f, -0.15f); glVertex3f(0.8f, -0.15f, 0.15f);
    // Left
    glVertex3f(-0.3f, 0.35f, -0.35f); glVertex3f(0.8f, 0.15f, -0.15f); glVertex3f(-0.3f, -0.35f, -0.35f);
    glVertex3f(-0.3f, -0.35f, -0.35f); glVertex3f(0.8f, 0.15f, -0.15f); glVertex3f(0.8f, -0.15f, -0.15f);
    // Right
    glVertex3f(-0.3f, 0.35f, 0.35f); glVertex3f(-0.3f, -0.35f, 0.35f); glVertex3f(0.8f, 0.15f, 0.15f);
    glVertex3f(0.8f, 0.15f, 0.15f); glVertex3f(-0.3f, -0.35f, 0.35f); glVertex3f(0.8f, -0.15f, 0.15f);
    glEnd();

    // --- 2. DOBLE ALA (Avion Biplano 3D - Blancas y Amarillas) ---
    shaderProgram.setVec3("color", glm::vec3(0.95f, 0.95f, 0.95f)); // Blanco
    glBegin(GL_TRIANGLES);
    // Ala Inferior Izquierda (y = -0.2f)
    glVertex3f(-0.1f, -0.2f, 0.15f);
    glVertex3f(0.4f, -0.2f, 0.15f);
    glVertex3f(0.3f, -0.2f, 1.4f);
    glVertex3f(-0.1f, -0.2f, 0.15f);
    glVertex3f(0.3f, -0.2f, 1.4f);
    glVertex3f(0.0f, -0.2f, 1.3f);

    // Ala Inferior Derecha (y = -0.2f)
    glVertex3f(-0.1f, -0.2f, -0.15f);
    glVertex3f(0.3f, -0.2f, -1.4f);
    glVertex3f(0.4f, -0.2f, -0.15f);
    glVertex3f(-0.1f, -0.2f, -0.15f);
    glVertex3f(0.0f, -0.2f, -1.3f);
    glVertex3f(0.3f, -0.2f, -1.4f);

    // Ala Superior Izquierda (y = 0.5f)
    glVertex3f(-0.1f, 0.5f, 0.15f);
    glVertex3f(0.4f, 0.5f, 0.15f);
    glVertex3f(0.3f, 0.5f, 1.4f);
    glVertex3f(-0.1f, 0.5f, 0.15f);
    glVertex3f(0.3f, 0.5f, 1.4f);
    glVertex3f(0.0f, 0.5f, 1.3f);

    // Ala Superior Derecha (y = 0.5f)
    glVertex3f(-0.1f, 0.5f, -0.15f);
    glVertex3f(0.3f, 0.5f, -1.4f);
    glVertex3f(0.4f, 0.5f, -0.15f);
    glVertex3f(-0.1f, 0.5f, -0.15f);
    glVertex3f(0.0f, 0.5f, -1.3f);
    glVertex3f(0.3f, 0.5f, -1.4f);
    glEnd();

    // Bordes de ataque del ala en Amarillo
    shaderProgram.setVec3("color", glm::vec3(1.0f, 0.8f, 0.0f)); // Amarillo
    glBegin(GL_TRIANGLES);
    // Ala superior puntas
    glVertex3f(0.3f, 0.5f, 1.4f); glVertex3f(0.4f, 0.5f, 0.15f); glVertex3f(0.42f, 0.5f, 0.15f);
    glVertex3f(0.3f, 0.5f, -1.4f); glVertex3f(0.42f, 0.5f, -0.15f); glVertex3f(0.4f, 0.5f, -0.15f);
    // Ala inferior puntas
    glVertex3f(0.3f, -0.2f, 1.4f); glVertex3f(0.4f, -0.2f, 0.15f); glVertex3f(0.42f, -0.2f, 0.15f);
    glVertex3f(0.3f, -0.2f, -1.4f); glVertex3f(0.42f, -0.2f, -0.15f); glVertex3f(0.4f, -0.2f, -0.15f);
    glEnd();

    // --- Soporte de las alas (Montantes 3D - Gris metalizado) ---
    shaderProgram.setVec3("color", glm::vec3(0.5f, 0.53f, 0.55f));
    glBegin(GL_LINES);
    // Struts Izquierdos
    glVertex3f(0.1f, -0.2f, 1.1f); glVertex3f(0.1f, 0.5f, 1.1f);
    glVertex3f(0.3f, -0.2f, 1.1f); glVertex3f(0.3f, 0.5f, 1.1f);
    // Struts Derechos
    glVertex3f(0.1f, -0.2f, -1.1f); glVertex3f(0.1f, 0.5f, -1.1f);
    glVertex3f(0.3f, -0.2f, -1.1f); glVertex3f(0.3f, 0.5f, -1.1f);
    glEnd();

    // --- 3. CABINA (Cristal Celeste con volumen) ---
    shaderProgram.setVec3("color", glm::vec3(0.3f, 0.8f, 1.0f)); // Celeste brillante
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.35f, 0.35f, 0.0f);
    glVertex3f(0.15f, 0.35f, 0.18f);
    glVertex3f(0.15f, 0.35f, -0.18f);

    glVertex3f(-0.35f, 0.35f, 0.0f);
    glVertex3f(0.1f, 0.65f, 0.0f); // Cabina alta
    glVertex3f(0.15f, 0.35f, 0.18f);

    glVertex3f(-0.35f, 0.35f, 0.0f);
    glVertex3f(0.15f, 0.35f, -0.18f);
    glVertex3f(0.1f, 0.65f, 0.0f);

    glVertex3f(0.1f, 0.65f, 0.0f);
    glVertex3f(0.35f, 0.25f, -0.1f);
    glVertex3f(0.35f, 0.25f, 0.1f);

    glVertex3f(0.1f, 0.65f, 0.0f);
    glVertex3f(0.15f, 0.35f, -0.18f);
    glVertex3f(0.35f, 0.25f, -0.1f);

    glVertex3f(0.1f, 0.65f, 0.0f);
    glVertex3f(0.35f, 0.25f, 0.1f);
    glVertex3f(0.15f, 0.35f, 0.18f);
    glEnd();

    // --- 4. COLA (Aletas 3D gruesas) ---
    shaderProgram.setVec3("color", glm::vec3(0.85f, 0.15f, 0.15f)); // Rojo
    glBegin(GL_TRIANGLES);
    // Vertical
    glVertex3f(0.4f, 0.15f, 0.0f);
    glVertex3f(0.8f, 0.15f, 0.0f);
    glVertex3f(0.8f, 0.7f, 0.0f);
    glEnd();

    shaderProgram.setVec3("color", glm::vec3(0.95f, 0.95f, 0.95f)); // Blanco
    glBegin(GL_TRIANGLES);
    // Horizontal Izquierda
    glVertex3f(0.5f, 0.15f, 0.0f);
    glVertex3f(0.8f, 0.15f, 0.0f);
    glVertex3f(0.8f, 0.15f, 0.45f);

    // Horizontal Derecha
    glVertex3f(0.5f, 0.15f, 0.0f);
    glVertex3f(0.8f, 0.15f, -0.45f);
    glVertex3f(0.8f, 0.15f, 0.0f);
    glEnd();

    // --- 5. H�LICE GIRATORIA ---
    // Spinner
    shaderProgram.setVec3("color", glm::vec3(0.9f, 0.9f, 0.9f));
    glBegin(GL_TRIANGLES);
    glVertex3f(-1.32f, 0.0f, 0.0f);
    glVertex3f(-1.2f, 0.12f, 0.12f);
    glVertex3f(-1.2f, 0.12f, -0.12f);

    glVertex3f(-1.32f, 0.0f, 0.0f);
    glVertex3f(-1.2f, -0.12f, -0.12f);
    glVertex3f(-1.2f, -0.12f, 0.12f);

    glVertex3f(-1.32f, 0.0f, 0.0f);
    glVertex3f(-1.2f, -0.12f, 0.12f);
    glVertex3f(-1.2f, 0.12f, 0.12f);

    glVertex3f(-1.32f, 0.0f, 0.0f);
    glVertex3f(-1.2f, 0.12f, -0.12f);
    glVertex3f(-1.2f, -0.12f, -0.12f);
    glEnd();

    // Palas giratorias
    glm::mat4 propModel = glm::translate(planeModel, glm::vec3(-1.24f, 0.0f, 0.0f));
    propModel = glm::rotate(propModel, (float)glfwGetTime() * 30.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    shaderProgram.setMat4("model", propModel);
    shaderProgram.setVec3("color", glm::vec3(0.12f, 0.12f, 0.12f)); // Gris oscuro

    glBegin(GL_QUADS);
    // Pala 1
    glVertex3f(0.0f, -0.05f, -0.65f);
    glVertex3f(0.0f, 0.05f, -0.65f);
    glVertex3f(0.0f, 0.05f, 0.0f);
    glVertex3f(0.0f, -0.05f, 0.0f);
    // Pala 2
    glVertex3f(0.0f, -0.05f, 0.0f);
    glVertex3f(0.0f, 0.05f, 0.0f);
    glVertex3f(0.0f, 0.05f, 0.65f);
    glVertex3f(0.0f, -0.05f, 0.65f);
    glEnd();
}

void drawPanel2D(float x, float y, float width, float height, glm::vec3 color, float alphaVal, Shader& shaderProgram) {
    shaderProgram.use();
    shaderProgram.setBool("useTexture", false);
    shaderProgram.setVec3("color", color);
    shaderProgram.setFloat("alpha", alphaVal);
    
    glm::mat4 orthoProj = glm::ortho(0.0f, 1200.0f, 0.0f, 800.0f);
    shaderProgram.setMat4("projection", orthoProj);
    shaderProgram.setMat4("view", glm::mat4(1.0f));
    shaderProgram.setMat4("model", glm::mat4(1.0f));
    
    glBegin(GL_QUADS);
    glVertex3f(x, y, 0.0f);
    glVertex3f(x + width, y, 0.0f);
    glVertex3f(x + width, y + height, 0.0f);
    glVertex3f(x, y + height, 0.0f);
    glEnd();
    
    shaderProgram.setFloat("alpha", 1.0f);
}

// Función para dibujar una esfera perfecta y pasarle su color al shader
void drawCloudSphereWithGradient(glm::mat4 baseModel, glm::vec3 offset, float radius, Shader& shaderProgram) {
    glm::mat4 sphereModel = glm::translate(baseModel, offset);
    sphereModel = glm::scale(sphereModel, glm::vec3(radius));
    shaderProgram.setMat4("model", sphereModel);

    int lats = 16;
    int longs = 16;

    glm::vec3 colorTop(1.0f, 1.0f, 1.0f);
    glm::vec3 colorBottom(0.65f, 0.68f, 0.72f);

    for (int i = 1; i <= lats; i++) {
        float factorY = (float)i / lats;
        glm::vec3 ringColor = glm::mix(colorBottom, colorTop, factorY);
        shaderProgram.setVec3("color", ringColor);

        float lat0 = 3.14159265f * (-0.5f + (float)(i - 1) / lats);
        float z0 = sin(lat0);
        float r0 = cos(lat0);

        float lat1 = 3.14159265f * (-0.5f + (float)i / lats);
        float z1 = sin(lat1);
        float r1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= longs; j++) {
            float lng = 2.0f * 3.14159265f * (float)j / longs;
            float x = cos(lng);
            float y = sin(lng);

            glVertex3f(x * r1, y * r1, z1);
            glVertex3f(x * r0, y * r0, z0);
        }
        glEnd();
    }
}


//NUBES
void drawCloud(glm::mat4 baseModel, Shader& shaderProgram) {
    drawCloudSphereWithGradient(baseModel, glm::vec3(0.0f, 0.0f, 0.0f), 0.65f, shaderProgram);
    drawCloudSphereWithGradient(baseModel, glm::vec3(-0.5f, -0.05f, 0.05f), 0.45f, shaderProgram);
    drawCloudSphereWithGradient(baseModel, glm::vec3(0.5f, -0.05f, -0.05f), 0.48f, shaderProgram);
    drawCloudSphereWithGradient(baseModel, glm::vec3(0.0f, 0.35f, 0.0f), 0.45f, shaderProgram);
    drawCloudSphereWithGradient(baseModel, glm::vec3(-0.1f, -0.2f, -0.1f), 0.5f, shaderProgram);
}

// --- NUEVA FUNCIÓN PARA EL SOL TEXTURIZADO GIRATORIO ---
void drawSun3D(glm::mat4 baseModel, float rotationTime, float radius, unsigned int textureID, Shader& shaderProgram) {
    glm::mat4 sunModel = glm::translate(baseModel, glm::vec3(0.0f)); // posici�n base
    sunModel = glm::rotate(sunModel, rotationTime * 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
    sunModel = glm::scale(sunModel, glm::vec3(radius)); // <--- Aqu� se aplica el tama�o chico
    shaderProgram.setMat4("model", sunModel);

    // Activamos las texturas en el Fragment Shader
    shaderProgram.setBool("useTexture", true);
    shaderProgram.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f)); // Color neutro para no alterar la imagen

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shaderProgram.setInt("texture1", 0);

    int lats = 20;
    int longs = 20;

    for (int i = 1; i <= lats; i++) {
        float lat0 = 3.14159265f * (-0.5f + (float)(i - 1) / lats);
        float z0 = sin(lat0);
        float r0 = cos(lat0);

        float lat1 = 3.14159265f * (-0.5f + (float)i / lats);
        float z1 = sin(lat1);
        float r1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= longs; j++) {
            float lng = 2.0f * 3.14159265f * (float)j / longs;
            float x = cos(lng);
            float y = sin(lng);

            float u = (float)j / longs;
            float v0 = (float)(i - 1) / lats;
            float v1 = (float)i / lats;

            // CORRECCI�N MODERNA: Pasamos la coordenada UV al atributo layout 1
            glVertexAttrib2f(1, u, v1);
            glVertex3f(x * r1, y * r1, z1);

            glVertexAttrib2f(1, u, v0);
            glVertex3f(x * r0, y * r0, z0);
        }
        glEnd();
    }
    // Desactivamos la textura para los siguientes renders (como el avi�n)
    shaderProgram.setBool("useTexture", false);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawGround() {
    glBegin(GL_QUADS);
    glVertex3f(-50.0f, -4.0f, -50.0f); glVertex3f(50.0f, -4.0f, -50.0f);
    glVertex3f(50.0f, -4.0f, 50.0f); glVertex3f(-50.0f, -4.0f, 50.0f);
    glEnd();
}

void input(GLFWwindow* w) {
    if (!isGameStarted && glfwGetKey(w, GLFW_KEY_ENTER) == GLFW_PRESS) {
        isGameStarted = true;
        return;
    }
    if (isPaused && glfwGetKey(w, GLFW_KEY_ENTER) == GLFW_PRESS) {
        isPaused = false;
        currentWordIndex = (currentWordIndex + 1) % levelWords.size();
        if (completedWords >= 5) {
            isLevelCleared = true;
        }
    }
    if (isLevelCleared && glfwGetKey(w, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (currentLevel < 3) {
            currentLevel++;
            if (currentLevel == 2) {
                levelWords = level2Words;
                speed = 17.0f;
            } else if (currentLevel == 3) {
                levelWords = level3Words;
                speed = 22.0f;
            }
            completedWords = 0;
            currentWordIndex = 0;
            lives = 3;
            activeObjects.clear();
            planePos = glm::vec3(0.0f, 5.0f, 0.0f);
            isLevelCleared = false;
        } else {
            // Reiniciar juego completo al terminar nivel 3
            currentLevel = 1;
            levelWords = level1Words;
            speed = 12.0f;
            completedWords = 0;
            currentWordIndex = 0;
            lives = 3;
            activeObjects.clear();
            planePos = glm::vec3(0.0f, 5.0f, 0.0f);
            isLevelCleared = false;
            isGameStarted = false;
        }
    }
    if (isGameOver && glfwGetKey(w, GLFW_KEY_ENTER) == GLFW_PRESS) {
        currentLevel = 1;
        levelWords = level1Words;
        speed = 12.0f;
        completedWords = 0;
        currentWordIndex = 0;
        lives = 3;
        activeObjects.clear();
        planePos = glm::vec3(0.0f, 5.0f, 0.0f);
        isGameOver = false;
        isGameStarted = false;
    }
    if (isGameStarted && !isPaused && !isGameOver && !isLevelCleared) {
        if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS && planePos.y < 8.0f)
            planePos.y += 7.0f * deltaTime;
        if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS && planePos.y > 2.5f)
            planePos.y -= 7.0f * deltaTime;
    }
}

int main() {
    srand(time(NULL));
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, " Piloto 3D - Aprendemos Ingles", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    Shader shader("res/Shader/vertexShader.glsl", "res/Shader/fragmentShader.glsl");
    Shader textShader("res/Shader/vertexText.glsl", "res/Shader/fragmentText.glsl");
    text.Init();
    text.LoadFont("fonts/arial.ttf");
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f); // Fondo celeste del cielo

    // --- CARGA DE LA TEXTURA DEL SOL ---
    unsigned int sunTexture;
    glGenTextures(1, &sunTexture);
    glBindTexture(GL_TEXTURE_2D, sunTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    // Forzamos la inversión vertical de la imagen para que cargue al derecho
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("res/texture/sun.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        cout << "Error al abrir la textura en res/texture/sun.jpg" << endl;
    }
    stbi_image_free(data);

    while (!glfwWindowShouldClose(window)) {
        float current = glfwGetTime();
        deltaTime = current - lastFrame;
        lastFrame = current;

        input(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- MATRICES 3D ---
        shader.use();
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 4.0f, 15.0f), glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("projection", proj);
        shader.setMat4("view", view);

        // --- RENDERIZAR ESCENARIO ---
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        shader.setVec3("color", glm::vec3(0.0f, 0.7f, 0.2f)); // Suelo Verde
        drawGround();

        // --- DIBUJAR JUGADOR (AVIÓN ROTADO) ---
        glm::mat4 planeModel = glm::mat4(1.0f);
        planeModel = glm::translate(planeModel, planePos);
        planeModel = glm::rotate(planeModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        planeModel = glm::rotate(planeModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

                drawDetailedAirplane(planeModel, shader);

        // --- LÓGICA DE APARICIÓN EN EL CIELO ---
        if (isGameStarted && !isPaused && !isGameOver && !isLevelCleared) {
            spawnTimer += deltaTime;
            if (spawnTimer >= SPAWN_INTERVAL) {
                spawnTimer = 0.0f;
                GameObject newObj;
                float skyY = 3.5f + (rand() % 41) / 10.0f;
                newObj.position = glm::vec3(15.0f, skyY, 0.0f);
                newObj.isCircle = (rand() % 3 == 0);
                newObj.active = true;
                activeObjects.push_back(newObj);
            }
        }

        // --- CONTROL Y DIBUJO DE OBSTÁCULOS ---
        for (size_t i = 0; i < activeObjects.size(); i++) {
            if (!activeObjects[i].active) continue;

            if (isGameStarted && !isPaused && !isGameOver && !isLevelCleared) {
                activeObjects[i].position.x -= speed * deltaTime;
            }

            glm::mat4 obstacleModel = glm::translate(glm::mat4(1.0f), activeObjects[i].position);

            if (activeObjects[i].isCircle) {
                // LLAMADA CORREGIDA: Dibuja el Sol 3D con textura y rotación horizontal continua
                drawSun3D(obstacleModel, (float)glfwGetTime(), 0.5f, sunTexture, shader);
            }
            else {
                drawCloud(obstacleModel, shader);
            }

            if (activeObjects[i].position.x < -12.0f) {
                activeObjects[i].active = false;
                continue;
            }

            //// --- DETECCIÓN DE COLISIONES ---
            //if (!isPaused && !isGameOver && !isLevelCleared) {
            //    if (abs(activeObjects[i].position.x - planePos.x) < 1.1f &&
            //        abs(activeObjects[i].position.y - planePos.y) < 1.1f) {

            //        activeObjects[i].active = false;

            //        if (activeObjects[i].isCircle) {
            //            isPaused = true;
            //            completedWords++;
            //            if (completedWords >= 5) { isLevelCleared = true; isPaused = false; }
            //        }
            //        else {
            //            lives--;
            //            if (lives <= 0) isGameOver = true;
            //        }
            //    }
            //}
            // --- DETECCIÓN DE COLISIONES (LÓGICA INVERTIDA) ---
            if (isGameStarted && !isPaused && !isGameOver && !isLevelCleared) {
                if (abs(activeObjects[i].position.x - planePos.x) < 1.1f &&
                    abs(activeObjects[i].position.y - planePos.y) < 1.1f) {

                    activeObjects[i].active = false;

                    if (activeObjects[i].isCircle) {
                        // ¡AHORA EL SOL QUITA VIDAS!
                        lives--;
                        if (lives <= 0) isGameOver = true;
                    }
                    else {
                        // ¡AHORA LA NUBE DESBLOQUEA LA PALABRA!
                        isPaused = true;
                        completedWords++;
                    }
                }
            }



        }

        activeObjects.erase(std::remove_if(activeObjects.begin(), activeObjects.end(),
            [](const GameObject& o) { return !o.active; }), activeObjects.end());

        // --- INTERFAZ DE TEXTO 2D ---
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 1. DIBUJAR PANELES DE FONDO (GUI)
        // Panel de Inicio
        if (!isGameStarted) {
            // Borde dorado
            drawPanel2D(295.0f, 185.0f, 610.0f, 430.0f, glm::vec3(1.0f, 0.84f, 0.0f), 1.0f, shader);
            // Cuerpo
            drawPanel2D(300.0f, 190.0f, 600.0f, 420.0f, glm::vec3(0.08f, 0.1f, 0.12f), 0.95f, shader);
        }

        // Panel HUD (Solo cuando el juego ha comenzado)
        if (isGameStarted) {
            drawPanel2D(15.0f, 650.0f, 280.0f, 135.0f, glm::vec3(0.08f, 0.1f, 0.12f), 0.75f, shader);
        }
        
        // Panel de Pausa (Palabra)
        if (isPaused && !isGameOver && !isLevelCleared) {
            // Borde dorado
            drawPanel2D(335.0f, 275.0f, 530.0f, 270.0f, glm::vec3(1.0f, 0.84f, 0.0f), 1.0f, shader);
            // Cuerpo
            drawPanel2D(340.0f, 280.0f, 520.0f, 260.0f, glm::vec3(0.08f, 0.1f, 0.12f), 0.95f, shader);
        }

        // Panel de Game Over
        if (isGameOver) {
            // Borde rojo
            drawPanel2D(345.0f, 355.0f, 510.0f, 210.0f, glm::vec3(0.8f, 0.1f, 0.1f), 1.0f, shader);
            // Cuerpo
            drawPanel2D(350.0f, 360.0f, 500.0f, 200.0f, glm::vec3(0.08f, 0.1f, 0.12f), 0.95f, shader);
        }

        // Panel de Nivel Completado
        if (isLevelCleared) {
            // Borde verde o dorado
            glm::vec3 borderColor = (currentLevel < 3) ? glm::vec3(0.0f, 0.8f, 0.3f) : glm::vec3(1.0f, 0.84f, 0.0f);
            drawPanel2D(315.0f, 155.0f, 570.0f, 570.0f, borderColor, 1.0f, shader);
            drawPanel2D(320.0f, 160.0f, 560.0f, 560.0f, glm::vec3(0.08f, 0.1f, 0.12f), 0.95f, shader);
        }

        // 2. DIBUJAR TEXTO (sobre los paneles)
        textShader.use();
        glm::mat4 orthoProj = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
        textShader.setMat4("projection", orthoProj);

        // Panel de Inicio
        if (!isGameStarted) {
            text.RenderText(textShader.ID, "PILOTO 3D: APRENDEMOS INGLES", 330, 540, 0.7f, glm::vec3(1.0f, 0.84f, 0.0f));
            text.RenderText(textShader.ID, "Bienvenido piloto! Instrucciones de vuelo:", 330, 480, 0.45f, glm::vec3(0.9f, 0.9f, 0.9f));
            
            text.RenderText(textShader.ID, "- Usa [ W ] y [ S ] para subir y bajar el avion.", 340, 430, 0.45f, glm::vec3(0.9f, 0.9f, 0.9f));
            text.RenderText(textShader.ID, "- Esquiva los SOLES (te quitan 1 vida).", 340, 390, 0.45f, glm::vec3(1.0f, 0.3f, 0.3f));
            text.RenderText(textShader.ID, "- Toca las NUBES para aprender nuevas palabras.", 340, 350, 0.45f, glm::vec3(0.2f, 0.8f, 1.0f));
            text.RenderText(textShader.ID, "- Completa 5 palabras para superar cada nivel.", 340, 310, 0.45f, glm::vec3(0.2f, 1.0f, 0.4f));
            
            text.RenderText(textShader.ID, "Presiona [ ENTER ] para despegar!", 380, 240, 0.55f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        // Stats del HUD (Solo cuando el juego ha comenzado)
        if (isGameStarted) {
            text.RenderText(textShader.ID, "VIDAS: " + to_string(lives), 35, 750, 0.8f, glm::vec3(1.0f, 0.2f, 0.2f));
            text.RenderText(textShader.ID, "PALABRAS: " + to_string(completedWords) + " / 5", 35, 710, 0.65f, glm::vec3(0.9f, 0.9f, 0.9f));
            text.RenderText(textShader.ID, "NIVEL: " + to_string(currentLevel), 35, 670, 0.65f, glm::vec3(1.0f, 0.84f, 0.0f));
        }

        if (isPaused && !isGameOver && !isLevelCleared) {
            text.RenderText(textShader.ID, "PALABRA DESBLOQUEADA", 370, 490, 0.7f, glm::vec3(1.0f, 0.84f, 0.0f));
            text.RenderText(textShader.ID, "Ingles: " + levelWords[currentWordIndex].english, 400, 430, 0.65f, glm::vec3(1.0f, 1.0f, 1.0f));
            text.RenderText(textShader.ID, "Espanol: " + levelWords[currentWordIndex].spanish, 400, 385, 0.65f, glm::vec3(0.2f, 0.8f, 1.0f));
            text.RenderText(textShader.ID, "Presiona [ENTER] para continuar", 365, 320, 0.45f, glm::vec3(0.2f, 1.0f, 0.4f));
        }

        if (isGameOver) {
            text.RenderText(textShader.ID, "GAME OVER", 480, 480, 1.2f, glm::vec3(1.0f, 0.2f, 0.2f));
            text.RenderText(textShader.ID, "Presiona [ENTER] para reiniciar", 380, 410, 0.55f, glm::vec3(0.9f, 0.9f, 0.9f));
        }

        if (isLevelCleared) {
            if (currentLevel < 3) {
                text.RenderText(textShader.ID, "NIVEL " + to_string(currentLevel) + " SUPERADO!", 410, 660, 0.8f, glm::vec3(0.2f, 1.0f, 0.4f));
                text.RenderText(textShader.ID, "Presiona [ENTER] para ir al Nivel " + to_string(currentLevel + 1), 350, 615, 0.5f, glm::vec3(0.9f, 0.9f, 0.9f));
                
                text.RenderText(textShader.ID, "Vocabulario aprendido:", 430, 530, 0.55f, glm::vec3(1.0f, 0.84f, 0.0f));
                int y = 475;
                for (const auto& w : levelWords) {
                    text.RenderText(textShader.ID, w.english + " = " + w.spanish, 460, y, 0.55f, glm::vec3(0.9f, 0.9f, 0.9f));
                    y -= 45;
                }
            } else {
                text.RenderText(textShader.ID, "JUEGO COMPLETADO!", 410, 520, 0.9f, glm::vec3(1.0f, 0.84f, 0.0f));
                text.RenderText(textShader.ID, "Felicitaciones! Has aprendido todo el vocabulario.", 340, 460, 0.5f, glm::vec3(0.9f, 0.9f, 0.9f));
                text.RenderText(textShader.ID, "Presiona [ENTER] para volver a jugar", 380, 400, 0.5f, glm::vec3(0.2f, 1.0f, 0.4f));
            }
        }

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
