#include "Building.h"

Building::Building(BuildingType buildingType, const string& objPath)
    : type(buildingType), modelPath(objPath), position(0.0f),
    rotation(0.0f), scale(1.0f), selected(false) {
}

Building::~Building() {
    // Base destructor - derived classes handle their own cleanup
}

BuildingType Building::getType() const {
    return type;
}

const string& Building::getModelPath() const {
    return modelPath;
}