// BuildingTypes.h
#pragma once
#include <string>

using namespace std;

enum class BuildingType {
    RESIDENTIAL,
    COMMERCIAL,
    INDUSTRIAL,
    PUBLIC,
    UTILITIES,
    PARKS,
    TRANSPORT
};

string buildingTypeToString(BuildingType type);