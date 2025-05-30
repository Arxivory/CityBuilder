#pragma once
#ifndef ROADMANAGER_H
#define ROADMANAGER_H
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Road.h"
#include "ShaderProgramCreator.h"

using namespace std;

class RoadManager {
protected:
	vector<unique_ptr<Road>> roads;
	ShaderProgramCreator shaderProgramCreator;
	GLuint shaderProgram;
	Road* selectedRoad = nullptr;

	void setupShaderProgram();

public:
	RoadManager();

	void init();
	
	virtual ~RoadManager();

	Road *selectRoad(const vec3& rayStart, const vec3& rayDir);

	Road *getSelectedRoad();

	Road* checkRoadSelection(const vec3& rayStart, const vec3& rayDir);

	void clearSelection();

	void addRoad(unique_ptr<Road> road);

	virtual void renderObjects(const mat4 &view, const mat4 &projection, const vec3& lightPos, const vec3& cameraPos);

};

#endif // !ROADMANAGER_H

