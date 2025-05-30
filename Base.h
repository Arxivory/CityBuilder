#pragma once
#ifndef BASE_H
#define BASE_H
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderProgramCreator.h"
#include "TextureLoader.h"

using namespace glm;
using namespace std;

class Base {
private:
	unsigned int baseVBO, baseVAO, baseEBO;
	unsigned int shaderProgram;
	GLuint textureID;
	ShaderProgramCreator shaderCreator;
	TextureLoader textureloader;
	bool initialized;
	void createBase();
	void setupShaderProgram();

public:
	Base();
	~Base();
	void init();
	void render(const mat4& view, const mat4& projection);
};

#endif // !BASE_H

