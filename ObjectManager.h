#pragma once
#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Building.h"
#include "ShaderProgramCreator.h"

using namespace std;

class ObjectManager {
protected:
	vector<unique_ptr<Building>> buildings;
	ShaderProgramCreator shaderProgramCreator;
	GLuint shaderProgram;
	Building *selectedBuilding = nullptr;

	void setupShaderProgram();

public:
	ObjectManager();

	void init();

	virtual ~ObjectManager();

	Building *selectBuilding(const vec3 &rayStart, const vec3 &rayDir);

	Building* getSelectedBuilding();

	Building* checkBuildingSelection(const glm::vec3& rayStart, const glm::vec3& rayDir);
	
	void clearSelection();

	void addBuilding(unique_ptr<Building> building);

	virtual void renderObjects(const mat4 &view, const mat4 &projection, const vec3& lightPos, const vec3& cameraPos);
};

#endif 
