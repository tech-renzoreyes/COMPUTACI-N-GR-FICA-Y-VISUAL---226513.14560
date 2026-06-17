//#version 330 core
//
//// Entradas desde el VBO
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
//
//// Salida hacia el Fragment Shader
//out vec3 ourColor;
//
//// Variables que reciben el desplazamiento desde C++
//uniform float xOffset;
//uniform float yOffset;
//
//void main() 
//{
//    // Aplicamos los offsets a las coordenadas x e y
//    gl_Position = vec4(aPos.x + xOffset, aPos.y + yOffset, aPos.z, 1.0);
//    
//    // Pasamos el color al Fragment Shader
//    ourColor = aColor;
//}

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform float xOffset;
uniform float yOffset;

void main()
{
   
    gl_Position = vec4(aPos.x + xOffset, aPos.y + yOffset, aPos.z, 1.0);


    ourColor = aColor;
}