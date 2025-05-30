#include "Road.h"

Road::Road(RoadType roadType, const string& objPath)
	: type(roadType), modelPath(objPath), position(0.0f),
	rotation(0.0f), scale(0.0f), selected(false) { }

Road::~Road() {}

RoadType Road::getType() const {
	return type;
}

const string& Road::getModelPath() const {
	return modelPath;
}