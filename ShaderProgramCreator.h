#pragma once
#ifndef SHADERPROGRAMCREATOR_H
#define SHADERPROGRAMCREATOR_H
#include <iostream>
#include <sstream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


using namespace std;

class ShaderProgramCreator {
protected:
	string loadShaderSource(const char* path);

public:
	GLuint createShaderProgram(const char *verPath, const char *fragPath);
};


#endif // SHADERLOADER_H

