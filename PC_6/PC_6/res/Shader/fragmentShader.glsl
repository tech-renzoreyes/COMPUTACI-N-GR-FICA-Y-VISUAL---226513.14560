#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 ourColor;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D planetTexture;
uniform bool isSun;
uniform bool useTexture;
uniform vec4 solidColor;

void main()
{
    if (!useTexture) {
        if (solidColor.r < 0.0) {
            FragColor = vec4(ourColor, 1.0);
        } else {
            FragColor = solidColor;
        }
    } else {
        vec4 texColor = texture(planetTexture, TexCoords);
        if (isSun) {
            FragColor = texColor;
        } else {
            // Lighting: Light source is at (0,0,0) (the Sun)
            vec3 lightPos = vec3(0.0, 0.0, 0.0);
            vec3 lightDir = normalize(lightPos - FragPos);
            
            // Diffuse
            float diff = max(dot(Normal, lightDir), 0.0);
            
            // Ambient
            vec3 ambient = 0.15 * texColor.rgb;
            
            // Diffuse color
            vec3 diffuse = diff * texColor.rgb;
            
            FragColor = vec4(ambient + diffuse, 1.0);
        }
    }
}