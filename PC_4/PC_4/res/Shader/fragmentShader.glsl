#version 330 core 
 
out vec4 FragColor; 
in vec3 ourColor; 
 
uniform float turboActivo; //variable qu hara cambiar de color a la nave
 
void main() 
{ 
    if(turboActivo == 1.0) 
    { 
        FragColor = vec4(1.0, 0.1, 0.1, 1.0); // rojo 
    } 
    else 
    { 
        FragColor = vec4(0.1, 0.3, 1.0, 1.0); // azul 
    } 
 } 