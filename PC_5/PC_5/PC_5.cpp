#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <stack>
#include "src/Shader.h"
// Variables para las posiciones
float posX = 0.0f;
float posY = -0.8f; // Empezamos un poco más abajo para ver el árbol
float deltaTime = 0.0f;
float lastFrame = 0.0f;
void responsive(GLFWwindow* window, int width, int height);
void userInput(GLFWwindow* window);
// L-System - Recursividad
std::string generateLSystem(std::string axiom, int iterations) {
	std::string current = axiom;
	for (int i = 0; i < iterations; i++) {
		std::string next = "";
		for (char c : current) {
			if (c == 'F') {
				// Regla principal del árbol fractal
				next += "F[+F]F[-F][F]";
			}
			else {
				next += c;
			}
		}
		current = next;
	}
	return current;
}
// Variables para las ramas
struct TurtleState {
	float x, y, angle;
};
// Generación de los vértices
std::vector<float> generateVertices(const std::string& sentence, float length, float angleDeg) {
	std::vector<float> vertices;
	std::stack<TurtleState> stack;
	float x = 0.0f;
	float y = 0.0f;
	float angle = 90.0f; // Mirando hacia arriba
	// Convertir a radianes para math.h
	float angleRad = angle * (3.14159265359f / 180.0f);
	for (char c : sentence) {
		if (c == 'F') {
			// Calcular nueva posición usando trigonometría
			float newX = x + length * std::cos(angleRad);
			float newY = y + length * std::sin(angleRad);
			// Agregar vértice inicial (Posición + Color Verde)
			vertices.push_back(x); vertices.push_back(y); vertices.push_back(0.0f);
			vertices.push_back(0.2f); vertices.push_back(0.8f); vertices.push_back(0.2f);
			// Agregar vértice final (Posición + Color Verde)
			vertices.push_back(newX); vertices.push_back(newY); vertices.push_back(0.0f);
			vertices.push_back(0.2f); vertices.push_back(0.8f); vertices.push_back(0.2f);
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
			stack.push({ x, y, angle });
		}
		else if (c == ']') {
			TurtleState state = stack.top();
			stack.pop();
			x = state.x;
			y = state.y;
			angle = state.angle;
			angleRad = angle * (3.14159265359f / 180.0f);
		}
	}
	return vertices;
}
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(1200, 800, "L-System Fractal", NULL, NULL);
	if (!window) { std::cout << "Error\n"; glfwTerminate(); return -1; }
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, responsive);
	if (glewInit() != GLEW_OK) { std::cout << "Error Glew\n"; glfwTerminate(); return -1; }
	Shader myShader("res/Shader/vertexShader.glsl", "res/Shader/fragmentShader.glsl");
	// Generamos las instrucciones del fractal una sola vez (4 iteraciones)
	std::string fractalInstructions = generateLSystem("F", 4);
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
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		userInput(window);
		// Calculamos una oscilación basada en el tiempo actual usando seno.
		float windEffect = std::sin(currentFrame * 1.5f) * 2.5f;
		float dynamicAngle = 25.0f + windEffect; // El ángulo base de 25° ahora oscilará
		// Generamos los vértices con el nuevo ángulo iterado
		std::vector<float> vertices = generateVertices(fractalInstructions, 0.05f, dynamicAngle);
		// Actualizamos el VBO en cada frame usando GL_DYNAMIC_DRAW
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
		// ----------------------------------------------
		myShader.use();
		myShader.setFloat("xOffset", posX);
		myShader.setFloat("yOffset", posY);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f); // Fondo oscuro
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(VAO);
		// El número de vértices es el tamaño del vector / 6 datos por vértice
		glDrawArrays(GL_LINES, 0, vertices.size() / 6);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}
void responsive(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
void userInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	float cameraSpeed = 1.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) posY -= cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) posY += cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) posX += cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) posX -= cameraSpeed;
}