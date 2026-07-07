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

// NOTA: Para cargar el JPG, necesitas incluir stb_image. 
// Si no la tienes instalada, asegúrate de añadir el archivo "stb_image.h" a tu proyecto.
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

struct WordPair {
    string english;
    string spanish;
};

// Tus 5 palabras del vocabulario para el Nivel 1
vector<WordPair> levelWords = {
    {"apple", "manzana"},
    {"dog", "perro"},
    {"cat", "gato"},
    {"sun", "sol"},
    {"book", "libro"}
};
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
void drawAirplane() {
    // --- CUERPO / FUSELAJE AVION ---
    glBegin(GL_TRIANGLES);
    glVertex3f(-1.2f, 0.0f, 0.0f); // Punta
    glVertex3f(0.6f, 0.2f, 0.2f); // Atrás arriba derecha
    glVertex3f(0.6f, 0.2f, -0.2f); // Atrás arriba izquierda

    glVertex3f(-1.2f, 0.0f, 0.0f);
    glVertex3f(0.6f, -0.2f, -0.2f);
    glVertex3f(0.6f, -0.2f, 0.2f);

    glVertex3f(-1.2f, 0.0f, 0.0f);
    glVertex3f(0.6f, -0.2f, 0.2f);
    glVertex3f(0.6f, 0.2f, 0.2f);

    glVertex3f(-1.2f, 0.0f, 0.0f);
    glVertex3f(0.6f, 0.2f, -0.2f);
    glVertex3f(0.6f, -0.2f, -0.2f);
    glEnd();

    // --- ALAS ---
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.2f, 0.0f, 0.0f);
    glVertex3f(0.4f, 0.0f, 0.0f);
    glVertex3f(0.4f, 0.0f, 0.6f);

    glVertex3f(-0.2f, 0.0f, 0.0f);
    glVertex3f(0.4f, 0.0f, 0.0f);
    glVertex3f(0.4f, 0.0f, -0.6f);
    glEnd();

    // --- COLA ---
    glBegin(GL_TRIANGLES);
    glVertex3f(0.2f, 0.0f, 0.0f);
    glVertex3f(0.6f, 0.0f, 0.0f);
    glVertex3f(0.6f, 0.4f, 0.0f);
    glEnd();
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

    for (int i = 0; i <= lats; i++) {
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
            float lng = 2.0f * 3.14159265f * (float)(j - 1) / longs;
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
    glm::mat4 sunModel = glm::translate(baseModel, glm::vec3(0.0f)); // posición base
    sunModel = glm::rotate(sunModel, rotationTime * 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
    sunModel = glm::scale(sunModel, glm::vec3(radius)); // <--- Aquí se aplica el tamaño chico
    shaderProgram.setMat4("model", sunModel);

    // Activamos las texturas en el Fragment Shader
    shaderProgram.setBool("useTexture", true);
    shaderProgram.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f)); // Color neutro para no alterar la imagen

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shaderProgram.setInt("texture1", 0);

    int lats = 20;
    int longs = 20;

    for (int i = 0; i <= lats; i++) {
        float lat0 = 3.14159265f * (-0.5f + (float)(i - 1) / lats);
        float z0 = sin(lat0);
        float r0 = cos(lat0);

        float lat1 = 3.14159265f * (-0.5f + (float)i / lats);
        float z1 = sin(lat1);
        float r1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= longs; j++) {
            float lng = 2.0f * 3.14159265f * (float)(j - 1) / longs;
            float x = cos(lng);
            float y = sin(lng);

            float u = (float)(j - 1) / longs;
            float v0 = (float)(i - 1) / lats;
            float v1 = (float)i / lats;

            // CORRECCIÓN MODERNA: Pasamos la coordenada UV al atributo layout 1
            glVertexAttrib2f(1, u, v1);
            glVertex3f(x * r1, y * r1, z1);

            glVertexAttrib2f(1, u, v0);
            glVertex3f(x * r0, y * r0, z0);
        }
        glEnd();
    }
    // Desactivamos la textura para los siguientes renders (como el avión)
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
    if (isPaused && glfwGetKey(w, GLFW_KEY_ENTER) == GLFW_PRESS) {
        isPaused = false;
        currentWordIndex = (currentWordIndex + 1) % levelWords.size();
    }
    if (!isPaused && !isGameOver && !isLevelCleared) {
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

        shader.setMat4("model", planeModel);
        shader.setVec3("color", glm::vec3(0.8f, 0.1f, 0.1f)); // Color del avión
        drawAirplane();

        // --- LÓGICA DE APARICIÓN EN EL CIELO ---
        if (!isPaused && !isGameOver && !isLevelCleared) {
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

            if (!isPaused && !isGameOver && !isLevelCleared) {
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
            if (!isPaused && !isGameOver && !isLevelCleared) {
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
                        if (completedWords >= 5) { isLevelCleared = true; isPaused = false; }
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

        textShader.use();
        glm::mat4 orthoProj = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
        textShader.setMat4("projection", orthoProj);

        text.RenderText(textShader.ID, "VIDAS: " + to_string(lives), 30, 750, 0.9f, glm::vec3(1.0f, 0.0f, 0.0f));
        text.RenderText(textShader.ID, "PALABRAS: " + to_string(completedWords) + " / 5", 30, 710, 0.7f, glm::vec3(1.0f, 1.0f, 1.0f));

        if (isPaused && !isGameOver && !isLevelCleared) {
            text.RenderText(textShader.ID, "PALABRA DESBLOQUEADA", 800, 750, 0.55f, glm::vec3(1.0f, 1.0f, 0.0f));
            text.RenderText(textShader.ID, "Ingles: " + levelWords[currentWordIndex].english, 850, 715, 0.55f, glm::vec3(1.0f, 0.0f, 0.0f));
            text.RenderText(textShader.ID, "Espanol: " + levelWords[currentWordIndex].spanish, 850, 685, 0.55f, glm::vec3(0.0f, 0.0f, 0.5f));
            text.RenderText(textShader.ID, "Presiona [ENTER] para continuar", 810, 650, 0.45f, glm::vec3(0.0f, 1.0f, 0.0f));
        }

        if (isGameOver) {
            text.RenderText(textShader.ID, "GAME OVER", 430, 630, 1.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        }

        if (isLevelCleared) {
            text.RenderText(textShader.ID, "NIVEL 1 SUPERADO", 630, 720, 0.9f, glm::vec3(0.0f, 1.0f, 0.3f));
            int y = 680;
            for (const auto& w : levelWords) {
                text.RenderText(textShader.ID, w.english + " = " + w.spanish, 680, y, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
                y -= 35;
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
