#include "RoadManager.h"
#include <iostream>

using namespace std;

void RoadManager::setupShaderProgram() {
	shaderProgram = shaderProgramCreator.createShaderProgram("shaders/vertex/RoadVertexShader.glsl",
		"shaders/fragment/RoadFragmentShader.glsl");
}

RoadManager::RoadManager() {}

void RoadManager::init() {
	setupShaderProgram();
}

RoadManager::~RoadManager() {}

Road* RoadManager::selectRoad(const vec3& rayStart, const vec3& rayDir) {
	selectedRoad = nullptr;
	for (auto& road : roads) {
		road->selected = false;
		if (road->intersects(rayStart, rayDir)) {
			selectedRoad = road.get();
			road->selected = true;
			break;
		}
	}
	return selectedRoad;
}

Road* RoadManager::getSelectedRoad() {
	return selectedRoad;
}

Road* RoadManager::checkRoadSelection(const vec3& rayStart, const vec3& rayDir) {
	for (auto& road : roads) {
		if (road && road->intersects(rayStart, rayDir)) {
			return road.get();
		}
	}
	return nullptr;
}

void RoadManager::clearSelection() {
	if (selectedRoad) {
		selectedRoad->selected = false;
		selectedRoad = nullptr;
	}
}

void RoadManager::addRoad(unique_ptr<Road> road) {
	roads.push_back(move(road));
}

void RoadManager::renderObjects(const mat4& view, const mat4& projection, const vec3& lightPos, const vec3& cameraPos) {
	for (auto& road : roads) {
		if (road) {
			road->render(shaderProgram, view, projection, lightPos, cameraPos);
		}
	}
}