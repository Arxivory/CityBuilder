#include "Base.h"
#include <iostream>
using namespace std;

Base::Base()
    : baseVBO(0), baseVAO(0), baseEBO(0), shaderProgram(0), textureID(0), initialized(false) {
}

Base::~Base() {
    // Clean up OpenGL resources
    if (baseVBO) glDeleteBuffers(1, &baseVBO);
    if (baseEBO) glDeleteBuffers(1, &baseEBO);
    if (baseVAO) glDeleteVertexArrays(1, &baseVAO);
    if (shaderProgram) glDeleteProgram(shaderProgram);
    if (textureID) glDeleteTextures(1, &textureID);
}

void Base::setupShaderProgram() {
    shaderProgram = shaderCreator.createShaderProgram("shaders/vertex/BaseVertexShader.glsl",
        "shaders/fragment/BaseFragmentShader.glsl");
    if (shaderProgram == 0) {
        cerr << "Failed to create base shader program" << endl;
        return;
    }
    cout << "Base shader program created successfully" << endl;
}

void Base::init() {
    if (initialized) {
        return;
    }

    setupShaderProgram();
    if (shaderProgram == 0) {
        cerr << "Failed to setup shader program" << endl;
        return;
    }

    textureID = textureloader.loadTexture("textures/base.jpg");
    if (textureID == 0) {
        cerr << "Failed to load base texture" << endl;
        return;
    }

    createBase();
    initialized = true;
    cout << "Base initialized successfully" << endl;
}

void Base::createBase() {
    // Ground plane vertices (Y=0 plane)
    float vertices[] = {
        // Positions          // Texture Coords
        -1.0f, 0.0f, -1.0f,   0.0f, 0.0f,  // Bottom-left
         1.0f, 0.0f, -1.0f,   1.0f, 0.0f,  // Bottom-right
         1.0f, 0.0f,  1.0f,   1.0f, 1.0f,  // Top-right
        -1.0f, 0.0f,  1.0f,   0.0f, 1.0f   // Top-left
    };

    unsigned int indices[] = {
        0, 1, 2,  // First triangle
        2, 3, 0   // Second triangle
    };

    // Generate and bind VAO
    glGenVertexArrays(1, &baseVAO);
    glBindVertexArray(baseVAO);

    // Generate and setup VBO
    glGenBuffers(1, &baseVBO);
    glBindBuffer(GL_ARRAY_BUFFER, baseVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generate and setup EBO
    glGenBuffers(1, &baseEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, baseEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute (location = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);
}

void Base::render(const mat4& view, const mat4& projection) {
    if (!initialized) {
        init();
        if (!initialized) {
            return; // Failed to initialize
        }
    }

    if (baseVAO == 0 || shaderProgram == 0) {
        cerr << "Base not properly initialized" << endl;
        return;
    }

    // Use shader program
    glUseProgram(shaderProgram);

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture uniform
    glUniform1i(glGetUniformLocation(shaderProgram, "baseTexture"), 0);

    // Calculate MVP matrix
    mat4 model = mat4(1.0f);
    model = scale(model, vec3(10.0f, 1.0f, 10.0f)); // Scale X and Z by 10, keep Y at 1

    mat4 mvp = projection * view * model;

    // Send MVP matrix to shader
    unsigned int mvpLoc = glGetUniformLocation(shaderProgram, "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, value_ptr(mvp));

    // Render the base
    glBindVertexArray(baseVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Unbind shader program
    glUseProgram(0);
}