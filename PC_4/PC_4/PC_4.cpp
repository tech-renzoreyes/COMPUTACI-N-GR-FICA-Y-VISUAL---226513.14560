#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include "src/Shader.h"
float vertices[] =
{
	//Posición //Color
	-0.5f,-0.5f,0.0f, 1.0f,0.0f,0.0f,
	0.5f,-0.5f,0.0f, 0.0f,1.0f,0.0f,
	0.0f, 0.5f,0.0f, 0.0f,0.0f,1.0f,
};
// Variables para las posiciones
float posX = 0.0f;
float posY = 0.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
void responsive(GLFWwindow* window, int width, int height);
void userInput(GLFWwindow* window);
void mouse_cursor_position(GLFWwindow* window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
int main()
{
	//Inicializar GLFW
	glfwInit();
	//Inicializar la version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	//Crear ventana
	GLFWwindow* window = glfwCreateWindow(1200, 800, "Mi primera ventana - Cristian", NULL, NULL);
	//Validar la ventana
	if (!window)
	{
		std::cout << "Error en al inicialziación la ventana\n";
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, responsive);
	glfwSetCursorPosCallback(window, mouse_cursor_position);
	glfwSetScrollCallback(window, mouse_scroll_callback);
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize Glew\n";
		glfwTerminate();
	}
	//Shader
	Shader myShader("res/Shader/vertexShader.glsl", "res/Shader/fragmentShader.glsl");
	//Buffers
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	//Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//Colores
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//Iniciamos
	while (!glfwWindowShouldClose(window))
	{
		//DeltaTime - Cambio del FPS
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//Control de Tecla
		userInput(window);
		//actualización de tiempos y colores
		float time = glfwGetTime();
		float xValue = std::cos(time) / 2.0f + 0.5f;
		// float yValue = std::sin(time) / 2.0f + 0.5f;
		// float zValue = std::cos(time) / 2.0f + 0.5f;
		myShader.use();
		myShader.setFloat("xColor", xValue);
		//Actualizar Posición
		myShader.setFloat("xOffset", posX);
		myShader.setFloat("yOffset", posY);
		//Render - Color
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// Traspaso de la pantalla
		glfwSwapBuffers(window);
		// cambio de eventos (congela)
		glfwPollEvents();
	}
	//limpiar
	glfwDestroyWindow(window);
	glfwTerminate();
}
void responsive(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void userInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	//Movimiento
	float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		posY = posY + cameraSpeed; // Arriba
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		posY -= cameraSpeed; // Abajo
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		posX -= cameraSpeed; // Izquierda
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		posX += cameraSpeed; // Derecha
}
void mouse_cursor_position(GLFWwindow* window, double xpos, double ypos)
{
	// std::cout << "Posición del Mouse :" << xpos << " " << ypos << std::endl;
}
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//std::cout << "Valor del Scroll: " << xoffset << " " << yoffset << std::endl;
}