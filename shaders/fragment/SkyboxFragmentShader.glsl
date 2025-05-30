#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D skyboxTexture;

void main() {
    vec3 color = texture(skyboxTexture, TexCoord).rgb;
    
    // Optional: Enhance the skybox colors
    color = pow(color, vec3(0.8)); // Slight gamma correction
    color *= 1.1; // Slight brightness boost
    
    FragColor = vec4(color, 1.0);
}