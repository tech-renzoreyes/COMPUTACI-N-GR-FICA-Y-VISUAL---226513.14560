#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos; 

uniform vec3 color;
uniform sampler2D texture1;
uniform bool useTexture; 
uniform float alpha = 1.0; 

void main()
{
    // Calculate flat normal using screen-space derivatives
    vec3 normal = normalize(cross(dFdx(FragPos), dFdy(FragPos)));
    
    // Ensure the normal faces the camera (positive Z in view space, or simply face-forwarding)
    if (normal.z < 0.0) {
        normal = -normal;
    }
    
    // Light direction (from top-right-front in world space)
    vec3 lightDir = normalize(vec3(0.5, 0.8, 1.0));
    
    // Ambient light
    float ambient = 0.40;
    
    // Diffuse light
    float diff = max(dot(normal, lightDir), 0.0) * 0.60;
    
    float lighting = ambient + diff;
    
    if (useTexture) {
        FragColor = texture(texture1, TexCoords) * vec4(color * lighting, alpha);
    } else {
        FragColor = vec4(color * lighting, alpha);
    }
}