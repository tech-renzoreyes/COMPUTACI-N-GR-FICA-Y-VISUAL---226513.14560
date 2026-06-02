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
int main()
{
	glfwInit();
	// Inicializar la version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	// Crear ventana
	GLFWwindow* window = glfwCreateWindow(1200, 800, "Mi primera ventana", NULL, NULL);
	// Validar la ventana
	if (!window)
	{
		std::cout << "Error en al inicialización de la ventana\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Validar el glew
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error en al inicialización del GLEW\n";
		glfwTerminate();
		return -1;
	}
	// Shader
	Shader myShader("res/Shader/vertexShader.glsl", "res/Shader/fragmentShader.glsl");
	// Buffers
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	// Vertices (Location 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Colores (Location 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Iniciamos el Game Loop
	while (!glfwWindowShouldClose(window))
	{
		// Actualización de tiempos
		float time = glfwGetTime();
		float xValue = std::sin(time) / 2.0f + 0.5f; // 0.0f a 1.0f
		// Calculamos el desplazamiento (offset) en el eje X para el movimiento
		float xOffset = std::sin(time) / 2.0f; // Oscilará entre -0.5 y 0.5
		// Render - Limpieza de pantalla
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// ACTIVAR EL SHADER ANTES DE ENVIAR UNIFORMS
		myShader.use();
		// Enviar variables uniform al shader
		myShader.setFloat("xColor", xValue);
		myShader.setFloat("xOffset", xOffset); // Enviamos el offset de movimiento
		// Dibujar el triángulo
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// Traspaso de la pantalla
		glfwSwapBuffers(window);
		// Cambio de eventos (inputs, etc)
		glfwPollEvents();
	}
	// Limpiar memoria
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}