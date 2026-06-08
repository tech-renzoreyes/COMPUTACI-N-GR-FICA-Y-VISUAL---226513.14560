#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform float xOffset;
uniform float yOffset;

void main()
{
    vec3 pos = aPos;

    // Triángulo superior
    if(aPos.y > 0.2)
    {
        pos.x += xOffset;
        pos.y -= yOffset;   // sentido contrario
    }
    else
    {
        pos.x += xOffset;
        pos.y += yOffset;   // sentido normal
    }

    gl_Position = vec4(pos, 1.0);
    ourColor = aColor;
}