#include "Skybox.h"
#include <iostream>
#include <vector>
#include <cmath>

#define M_PI 3.14159265359

using namespace std;
using namespace glm;

Skybox::Skybox()
    : sphereVBO(0), sphereVAO(0), sphereIndexCount(0),
    shaderProgram(0), textureID(0),
    dragging(false), dragStart(0.0f), transformStart(0.0f),
    initialized(false) {
}

Skybox::~Skybox() {
    if (sphereVAO != 0) {
        glDeleteVertexArrays(1, &sphereVAO);
        sphereVAO = 0;
    }

    if (sphereVBO != 0) {
        glDeleteBuffers(1, &sphereVBO);
        sphereVBO = 0;
    }

    if (shaderProgram != 0) {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }

    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }

    initialized = false;
}

void Skybox::init() {
    if (initialized) {
        return;
    }

    setupShaderProgram();

    textureID = textureloader.loadTexture("textures/piste.jpg"); 
    if (textureID == 0) {
        cerr << "Failed to load skybox texture" << endl;
        return;
    }

    createSkyboxSphere();

    initialized = true;
    cout << "Skybox initialized successfully" << endl;
}

void Skybox::createSkyboxSphere() {
    vector<float> vertices;
    vector<unsigned int> indices;

    const int rings = 30;
    const int sectors = 30;
    const float radius = 100.0f; 

    for (int r = 0; r <= rings; ++r) {
        float phi = M_PI * r / rings; // 0 to PI
        float y = radius * cos(phi);
        float ringRadius = radius * sin(phi);

        for (int s = 0; s <= sectors; ++s) {
            float theta = 2.0f * M_PI * s / sectors; // 0 to 2*PI
            float x = ringRadius * cos(theta);
            float z = ringRadius * sin(theta);

            // Position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Texture coordinates (UV mapping)
            float u = (float)s / sectors;
            float v = 1.0f - (float)r / rings; // Flip V coordinate if texture appears upside down
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    for (int r = 0; r < rings; ++r) {
        for (int s = 0; s < sectors; ++s) {
            int current = r * (sectors + 1) + s;
            int next = current + sectors + 1;

            // First triangle
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            // Second triangle
            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    sphereIndexCount = indices.size();

    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);

    unsigned int sphereEBO;
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
        vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
        indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Skybox::setupShaderProgram() {
    
    shaderProgram = shaderCreator.createShaderProgram("shaders/vertex/SkyboxVertexShader.glsl", 
        "shaders/fragment/SkyboxFragmentShader.glsl");

    if (shaderProgram == 0) {
        cerr << "Failed to create skybox shader program" << endl;
        return;
    }

    cout << "Skybox shader program created successfully" << endl;
}

void Skybox::render(const mat4& view, const mat4& projection) {
    if (!initialized) {
        init();
        if (!initialized) {
            return; // Failed to initialize
        }
    }

    if (sphereVAO == 0 || shaderProgram == 0) {
        cerr << "Skybox not properly initialized" << endl;
        return;
    }

    glDepthFunc(GL_LEQUAL); 
    glDepthMask(GL_FALSE);   

    glUseProgram(shaderProgram);

    mat4 model = mat4(1.0f);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, value_ptr(model));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "skyboxTexture"), 0);

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}