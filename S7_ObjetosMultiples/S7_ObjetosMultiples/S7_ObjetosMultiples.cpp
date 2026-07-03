#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

#include "src/Shader.h"
#include "src/Camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#define SCR_WIDTH 1000
#define SCR_HEIGHT 800
#define MAPSIZE_X 10
#define MAPSIZE_Y 10
#define MAPSIZE_Z 10

float vertices[] =
{
	//Posicion Superior		Color					TexCoords
	-0.5f,-0.5f, 0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
	 0.5f,-0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
	 0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
	 0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
	-0.5f, 0.5f, 0.5f,		1.0f, 1.0f, 0.0f,		0.0f, 1.0f,
	-0.5f,-0.5f, 0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
	//Posicion Inferior		Color					TexCoords
	-0.5f,-0.5f,-0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
	 0.5f,-0.5f,-0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
	 0.5f, 0.5f,-0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
	 0.5f, 0.5f,-0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
	-0.5f, 0.5f,-0.5f,		1.0f, 1.0f, 0.0f,		0.0f, 1.0f,
	-0.5f,-0.5f,-0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
	//Posicion Izquierda	Color					TexCoords
	-0.5f,-0.5f,-0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
	-0.5f, 0.5f,-0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
	-0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
	-0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
	-0.5f,-0.5f, 0.5f,		1.0f, 1.0f, 0.0f,		0.0f, 1.0f,
	-0.5f,-0.5f,-0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
	//Posicion Derecha		Color					TexCoords
	 0.5f,-0.5f,-0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
	 0.5f, 0.5f,-0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
	 0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
	 0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
	 0.5f,-0.5f, 0.5f,		1.0f, 1.0f, 0.0f,		0.0f, 1.0f,
	 0.5f,-0.5f,-0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
	 //Posicion Posterior	Color					TexCoords
	 -0.5f, 0.5f,-0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
	  0.5f, 0.5f,-0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
	  0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
	  0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
	 -0.5f, 0.5f, 0.5f,		1.0f, 1.0f, 0.0f,		0.0f, 1.0f,
	 -0.5f, 0.5f,-0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
	 //Posicion Frontal		Color					TexCoords
	 -0.5f,-0.5f,-0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
	  0.5f,-0.5f,-0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
	  0.5f,-0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
	  0.5f,-0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
	 -0.5f,-0.5f, 0.5f,		1.0f, 1.0f, 0.0f,		0.0f, 1.0f,
	 -0.5f,-0.5f,-0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,


};


void responsive(GLFWwindow* window, int width, int height);
void userInput(GLFWwindow* window);
void mouse_cursor_position(GLFWwindow* window, double xpos, double ypos);
void mouse_scroll_position(GLFWwindow* window, double xoffset, double yoffset);
unsigned int load_texture(const char* texture_path);

//Matrices
glm::mat4 model;
glm::mat4 projection;
glm::mat4 view;


//frames
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = float(SCR_WIDTH) / 2.0f;
float lastY = float(SCR_HEIGHT) / 2.0f;
bool isFirstMouse = true;

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
	GLFWwindow* window = glfwCreateWindow(1200, 800, "Mi primera ventana", NULL, NULL);

	//Validar la ventana
	if (!window)
	{
		std::cout << "Error en al inicialziación la ventana\n";
		glfwTerminate();
	}


	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, responsive);
	glfwSetCursorPosCallback(window, mouse_cursor_position);
	glfwSetScrollCallback(window, mouse_scroll_position);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Validar GLEW
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize Glew\n";
		glfwTerminate();
	}

	glEnable(GL_DEPTH_TEST);


	//Buffers
	unsigned int VBO, VAO;

	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	//vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Colores
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Textura Coordenadas
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	//Textura
	stbi_set_flip_vertically_on_load(true);
	unsigned int container_texture = load_texture("res/Texture/container.jpg");
	unsigned int face_texture = load_texture("res/Texture/awesomeface.png");
	GLuint pyramid_texture = load_texture("res/Texture/pyramid.jpg");

	//Shader
	Shader myShader("res/Shader/vertexShader.glsl", "res/Shader/fragmentShader.glsl");
	myShader.use();
	myShader.setInt("container_texture", 0);
	myShader.setInt("face_texture", 1);
	myShader.setInt("pyramid_texture", 2);


	//Iinciamos
	while (!glfwWindowShouldClose(window))
	{
		//Cambios del FPS
		userInput(window);
		float time = glfwGetTime();
		deltaTime = time - lastFrame;
		lastFrame = time;
		float xValue = std::cos(time) / 2.0f + 0.5f; // 0.0f - 1.0f
		float yValue = std::cos(time) / 2.0f + 0.5f; // 0.0f - 1.0f
		float zValue = std::sin(time) / 2.0f + 0.5f; // 0.0f - 1.0f

		float radius = 5.0f;
		float camX = std::sin(time) * radius;
		float camZ = std::cos(time) * radius;

		myShader.setFloat("alpha", xValue);

		//vector
		glm::vec3 myVector;
		myVector.x = xValue;
		myVector.y = yValue;
		myVector.z = 0.31f;

		myShader.setVec3("colors", myVector);


		//Proyeccion
		projection = glm::perspective(glm::radians(camera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.0f);
		myShader.setMat4("projection", projection);

		//Vista
		view = glm::mat4(1.0f);
		view = camera.GetViewMatrix();
		myShader.setMat4("view", view);

		//Model
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(1.2f));
		myShader.setMat4("model", model);

		//Render Color
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f); // 0.0 - 1.0
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myShader.use();

		glBindVertexArray(VAO);
		glLineWidth(10.0f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, container_texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, face_texture);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Traspaso de la pantalla 
		glfwSwapBuffers(window);
		//cambio de eventos (congela)
		glfwPollEvents();
	}

	//Limpiar
	glfwDestroyWindow(window);
	glfwTerminate();

}

void responsive(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void userInput(GLFWwindow* window)
{
	const float camera_speed = 2.5f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	// CAMBIO: Se cambió la comparación '== GLFW_TRUE' por '== GLFW_PRESS'.
	// En GLFW, glfwGetKey retorna GLFW_PRESS (tecla presionada) o GLFW_RELEASE (tecla libre).
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) //Adelante
		camera.ProcessKeyboard(FORWARD, deltaTime);
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) //Hacia atrás
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) //Derecha
		camera.ProcessKeyboard(RIGHT, deltaTime);
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) //Izquierda
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) //Arriba
		camera.ProcessKeyboard(UP, deltaTime);
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) //Abajo
		camera.ProcessKeyboard(DOWN, deltaTime);

}
void mouse_cursor_position(GLFWwindow* window, double xpos, double ypos)
{
	if (isFirstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		isFirstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouse_scroll_position(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int load_texture(const char* texture_path)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//Filtros
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(texture_path, &width, &height, &nrChannels, 0);

	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		if (nrChannels == 3) // jpg file
			format = GL_RGB;
		if (nrChannels == 4) // png file
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Error al cargar la Textura\n";
	}

	stbi_image_free(data);

	return texture;
}