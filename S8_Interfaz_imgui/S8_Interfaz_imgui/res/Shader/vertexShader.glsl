#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoords;

out vec3 ourColor;
out vec2 TexCoords;

uniform mat4 model;
// CAMBIO: Declarar las matrices de proyección (projection) y vista (view)
// para poder recibir las transformaciones de perspectiva y de la cámara desde C++.
uniform mat4 view;
uniform mat4 projection;

void main()
{	
	TexCoords = aTexCoords;
	ourColor = aColor;
	// CAMBIO: Multiplicar la posición del vértice por la proyección y la vista
	// para que los movimientos de la cámara afecten la visualización en pantalla.
	gl_Position = projection * view * model * vec4(aPos , 1.0f);
}