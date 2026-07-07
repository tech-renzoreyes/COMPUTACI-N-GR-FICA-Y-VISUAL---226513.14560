//#version 330 core
//
//layout(location = 0) in vec3 aPos;
//
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//
//void main()
//{
//    gl_Position = projection * view * model * vec4(aPos, 1.0);
//}
#version 330 core
layout (location = 0) in vec3 aPos;
// Creamos nuestra propia entrada para la textura en la locación 1
layout (location = 1) in vec2 aTexCoords; 

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    // Pasamos el atributo directamente al Fragment Shader
    TexCoords = aTexCoords; 
}