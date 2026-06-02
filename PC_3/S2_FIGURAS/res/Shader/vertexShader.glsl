#version 330 core

// Entradas desde el VBO
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

// Salida hacia el Fragment Shader
out vec3 ourColor;

// Variable que recibe el desplazamiento desde C++
uniform float xOffset; 

void main()
{
    // Sumamos xOffset a la posición X original del vértice
    gl_Position = vec4(aPos.x + xOffset, aPos.y, aPos.z, 1.0);
    
    // Pasamos el color al Fragment Shader
    ourColor = aColor;
}