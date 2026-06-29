#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoords;

out vec3 ourColor;
out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform bool isStarField;

void main()
{
    if (isStarField) {
        ourColor = aColor;
        TexCoords = vec2(0.0);
        FragPos = vec3(0.0);
        Normal = vec3(0.0);
        gl_Position = projection * view * vec4(aPos, 1.0);
    } else {
        TexCoords = aTexCoords;
        ourColor = aColor;
        FragPos = vec3(model * vec4(aPos, 1.0f));
        // Using aPos directly as model-space normal since sphere is centered at (0,0,0) and has radius 1
        Normal = normalize(mat3(transpose(inverse(model))) * aPos);
        gl_Position = projection * view * model * vec4(aPos, 1.0f);
    }
}