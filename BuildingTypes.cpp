#include "BuildingTypes.h"

// BuildingTypes.cpp
std::string buildingTypeToString(BuildingType type) {
    switch (type) {
    case BuildingType::RESIDENTIAL: return "Residential";
    case BuildingType::COMMERCIAL: return "Commercial";
    case BuildingType::INDUSTRIAL: return "Industrial";
    case BuildingType::PUBLIC: return "Public";
    case BuildingType::UTILITIES: return "Utilities";
    case BuildingType::PARKS: return "Parks";
    case BuildingType::TRANSPORT: return "Transport";
    default: return "Unknown";
    }
}