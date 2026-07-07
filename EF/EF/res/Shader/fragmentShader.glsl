//#version 330 core
//
//out vec4 FragColor;
//
//uniform vec3 color;
//
//void main()
//{
//    FragColor = vec4(color, 1.0);
//}
//#version 330 core
//out vec4 FragColor;
//
//in vec2 TexCoords; // Coordenadas que vienen del vertex shader
//
//uniform vec3 color;
//uniform sampler2D texture1;
//uniform bool useTexture; // Flag para activar/desactivar texturas
//
//void main()
//{
//    if (useTexture) {
//        // Mapea la textura del sol combinada sutilmente con el brillo amarillo
//        FragColor = texture(texture1, TexCoords) * vec4(color, 1.0);
//    } else {
//        FragColor = vec4(color, 1.0);
//    }
//}


#version 330 core
out vec4 FragColor;

in vec2 TexCoords; // Viene del Vertex Shader

uniform vec3 color;
uniform sampler2D texture1;
uniform bool useTexture; // ¡Esta variable es clave!

void main()
{
    if (useTexture) {
        // Muestra la imagen multiplicada por el color (blanco neutro)
        FragColor = texture(texture1, TexCoords) * vec4(color, 1.0);
    } else {
        // Muestra color plano (para el avión y el suelo)
        FragColor = vec4(color, 1.0);
    }
}