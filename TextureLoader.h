#pragma once
#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H
#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class TextureLoader {
public:

	GLuint loadTexture(const char* path);

};

#endif // !TEXTURELOADER_H

