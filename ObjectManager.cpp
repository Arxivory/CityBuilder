#include "ObjectManager.h"
#include <iostream>

using namespace std;

void ObjectManager::setupShaderProgram() {
	shaderProgram = shaderProgramCreator.createShaderProgram("shaders/vertex/BuildingVertexShader.glsl", 
		"shaders/fragment/BuildingFragmentShader.glsl");
}

ObjectManager::ObjectManager() {}

void ObjectManager::init() {
	setupShaderProgram();
}

ObjectManager::~ObjectManager() {}

Building *ObjectManager::selectBuilding(const vec3& rayStart, const vec3& rayDir) {
	selectedBuilding = nullptr;
	for (auto& building : buildings) {
		building->selected = false;
		if (building->intersects(rayStart, rayDir)) {
			selectedBuilding = building.get();
			building->selected = true;
			break;
		}
	}
	return selectedBuilding;
}

Building* ObjectManager::getSelectedBuilding() {
	return selectedBuilding;
}

Building* ObjectManager::checkBuildingSelection(const glm::vec3& rayStart, const glm::vec3& rayDir) {
	// Check each building for intersection with the ray
	for (auto& building : buildings) {
		if (building && building->intersects(rayStart, rayDir)) {
			return building.get();
		}
	}
	return nullptr;
}

void ObjectManager::clearSelection() {
	if (selectedBuilding) {
		selectedBuilding->selected = false;
		selectedBuilding = nullptr;
	}
}

void ObjectManager::addBuilding(unique_ptr<Building> building) {
	buildings.push_back(move(building));
}

void ObjectManager::renderObjects(const mat4 &view, const mat4 &projection, const vec3& lightPos, const vec3& cameraPos) {
	for (auto& building : buildings) {
		if (building) {
			building->render(shaderProgram, view, projection, lightPos, cameraPos);
		}
	}
}