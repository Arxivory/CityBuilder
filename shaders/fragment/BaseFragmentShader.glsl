#version 330 core

// Input from vertex shader
in vec2 TexCoord;

// Output color
out vec4 FragColor;

// Uniforms
uniform sampler2D baseTexture;

void main()
{
    // Sample the texture
    vec4 texColor = texture(baseTexture, TexCoord);
    
    // Output the final color
    FragColor = texColor;
    
    // Optional: Add some basic lighting or color adjustments
    // FragColor = texColor * vec4(0.8, 0.9, 0.8, 1.0); // Slight green tint for grass
}