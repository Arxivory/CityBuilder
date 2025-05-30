#pragma once
#ifndef STRAIGHTROAD_H
#define STRAIGHTROAD_H
#include "Road.h"
#include "OBJLoader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>\

using namespace std;
using namespace glm;

class StraightRoad : public Road {
private:
	OBJLoader objModel;
	unsigned int VAO, VBO;
	bool isInitialized;

	void setupMesh();
	void cleanup();

public:
	StraightRoad(const vec3& pos);
	~StraightRoad();

	bool initialize();

	bool intersects(const vec3& rayStart, const vec3& rayDir) override;

	void render(unsigned int shaderProgram, const mat4& view,
		const mat4& projection, const vec3& lightPos, const vec3& cameraPos);
};

#endif // ! STRAIGHTROAD_H