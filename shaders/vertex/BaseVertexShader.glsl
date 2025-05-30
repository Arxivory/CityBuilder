#version 330 core

// Input vertex attributes
layout (location = 0) in vec3 aPos;        // Vertex position
layout (location = 1) in vec2 aTexCoord;   // Texture coordinates

// Output to fragment shader
out vec2 TexCoord;

// Uniforms
uniform mat4 mvp;  // Model-View-Projection matrix

void main()
{
    // Transform vertex position
    gl_Position = mvp * vec4(aPos, 1.0);
    
    // Pass texture coordinates to fragment shader
    TexCoord = aTexCoord;
}