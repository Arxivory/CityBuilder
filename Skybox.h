#pragma once
#ifndef SKYBOX_H
#define SKYBOX_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderProgramCreator.h"
#include "TextureLoader.h"

using namespace glm;
using namespace std;

class Skybox {
private:
    unsigned int sphereVBO, sphereVAO, sphereIndexCount;
    unsigned int shaderProgram;
    GLuint textureID;
    ShaderProgramCreator shaderCreator;
    TextureLoader textureloader;

    bool dragging;
    glm::vec2 dragStart;
    glm::vec3 transformStart;
    bool initialized;

    void createSkyboxSphere();
    void setupShaderProgram();

public:
    Skybox();
    ~Skybox();

    void init();

    void render(const mat4& view, const mat4& projection);
};

#endif // !SKYBOX_H
