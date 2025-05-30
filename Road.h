#pragma once
#ifndef ROAD_H
#define ROAD_H
#include "RoadTypes.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "BuildingTypes.h"

using namespace glm;
using namespace std;

class Road {
protected:
	RoadType type;
	string modelPath;

public:
	vec3 position, rotation, scale;
	bool selected;

	Road(RoadType roadType, const string& objPath);

	virtual ~Road();

	RoadType getType() const;
	const string& getModelPath() const;

	bool virtual intersects(const vec3& rayStart, const vec3& rayDir) = 0;

	virtual void render(unsigned int shaderProgram, const mat4& view,
		const mat4& projection, const vec3& lightPos, const vec3& cameraPos) = 0;

	virtual vec3 getPosition() { return position; }
	virtual void setPosition(const vec3 &pos) { position = pos; }
	virtual vec3 getRotation() { return rotation; }
	virtual void setRotation(const vec3 &rot) { rotation = rot; }
	virtual vec3 getScale() { return scale; }
	virtual void setScale(const vec3& scl) { scale = scl; }

};

#endif // !ROAD_H

