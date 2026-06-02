#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// MISIÓN 1: Optimización de Memoria
// MISIÓN 2: Geometría Enriquecida

// Tarea M1: Arreglo con solo 4 vértices únicos.
// Tarea M2: Atributos intercalados (Posición X,Y,Z y Color R,G,B en la misma fila)
float vertices[] = {
	// Posiciones         // Colores 
	 0.5f,  0.5f, 0.0f,   0.2f, 1.0f, 0.8f,  // Arriba derecha
	 0.5f, -0.5f, 0.0f,   0.8f, 0.2f, 1.0f,  // Abajo derecha
	-0.5f, -0.5f, 0.0f,   1.0f, 0.8f, 0.2f,  // Abajo izquierda
	-0.5f,  0.5f, 0.0f,   0.2f, 0.8f, 1.0f   // Arriba izquierda
};

// Tarea M1: Arreglo de índices creado (dos triángulos formados por los 4 vértices)
unsigned int indices[] = {
	0, 1, 3,  // Primer triángulo
	1, 2, 3   // Segundo triángulo
};

// SHADERS

// Tarea M2: Actualizar el Vertex Shader para recibir color (Location 1)
const char* VertexShaderData =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n" // Nuevo atributo recibido
"out vec3 miColor;\n" // Enviar al fragment shader
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0f);\n"
"   miColor = aColor;\n"
"}\0";

// Tarea M3: Modificar Fragment Shader con uniform y alterar opacidad
const char* FragmentShaderData =
"#version 330 core\n"
"in vec3 miColor;\n" // Color recibido del vertex shader
"out vec4 FragColor;\n"
"uniform float u_time;\n" // Variable uniform añadida
"void main()\n"
"{\n"
"   float pulsoAlpha = (sin(u_time * 3.0f) + 1.0f) / 2.0f;\n"
"   FragColor = vec4(miColor, 0.2f + (pulsoAlpha * 0.8f));\n"
"}\0";


int main()
{
	// Inicializar GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Crear ventana
	GLFWwindow* window = glfwCreateWindow(1200, 800, "TRABAJO DE CAMPO 2 - G3", NULL, NULL);
	if (!window)
	{
		std::cout << "Error en al inicialziación la ventana\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Validar el glew
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error en al inicialziación del GLEW\n";
		glfwTerminate();
		return -1;
	}

	// COMPILACIÓN DE SHADERS 

	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &VertexShaderData, NULL);
	glCompileShader(vertex);
	int success;
	char infolog[1024];
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 1024, NULL, infolog);
		std::cout << "Error en el Shader<" << infolog << ">" << std::endl;
	}

	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &FragmentShaderData, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 1024, NULL, infolog);
		std::cout << "Error en el Shader Fragment<" << infolog << ">" << std::endl;
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 1024, NULL, infolog);
		std::cout << "Error en el programa<" << infolog << ">" << std::endl;
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	// GESTIÓN DE BUFFERS

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Tarea M1: Implementar el EBO 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Tarea M2: Configurar punteros calculando Stride y Offset
	// 1. Puntero para la Posición (Location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// 2. Puntero para el Color (Location = 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Habilitar transparencias (Blend)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// BUCLE

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program);

		// Tarea M3: Capturar tiempo actual y enviarlo al Shader usando u_time
		float tiempoActual = glfwGetTime();
		int ubicacionUniform = glGetUniformLocation(program, "u_time");
		glUniform1f(ubicacionUniform, tiempoActual);

		glBindVertexArray(VAO);

		// Tarea M1: Cambiar glDrawArrays por glDrawElements
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Limpieza 
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(program);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}