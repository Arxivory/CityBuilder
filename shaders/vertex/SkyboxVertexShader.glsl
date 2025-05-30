#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() {
    // Remove translation from view matrix to keep skybox centered on camera
    mat4 viewNoTranslation = mat4(mat3(view));
    
    // Apply model transformation for the sphere
    gl_Position = projection * viewNoTranslation * model * vec4(aPos, 1.0);
    
    // Pass texture coordinates to fragment shader
    TexCoord = aTexCoord;
}