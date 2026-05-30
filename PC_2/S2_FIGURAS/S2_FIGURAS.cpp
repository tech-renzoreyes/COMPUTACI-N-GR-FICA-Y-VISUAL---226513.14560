#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
float vertices[] =
{
	// Primer triángulo (BL, BR, TR)
	-0.5f, -0.5f, 0.0f, // BL
	 0.5f, -0.5f, 0.0f, // BR
	 0.2f,  0.3f, 0.0f, // TR

	// Segundo triángulo (TR, TL, BL)
	  0.2f,  0.3f, 0.0f, // TR
	 -0.8f,  0.3f, 0.0f, // TL  (desplazado igual que TR respecto a la base)
	 -0.5f, -0.5f, 0.0f // BL
};
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow* window = glfwCreateWindow(800, 400, "Mi primera Ventana", NULL, NULL);
	if (!window)
	{
		std::cout << "Error al crear la ventana\n";
		glfwTerminate();
	}
	/* The most important part */
	glfwMakeContextCurrent(window);
	// Check Glew
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error al iniciar GLEW\n";
		glfwTerminate();
	}
	//Buffers - pintor de los graficos
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);//configuracion del vertice
	glGenBuffers(1, &VBO);//alamacen de memoria
	glBindVertexArray(VAO);//Guardamos la informacion de los vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//usamos el lguar de pintado
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);//volcamos los puntos de los vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//Agrupamos de 3 en 3
	glEnableVertexAttribArray(0);//Guardando la memoria de la carpeta
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f); // 0.0f - 1.0f
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(VAO);
		//cant triangulo
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}