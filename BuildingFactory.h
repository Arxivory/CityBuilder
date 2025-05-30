#pragma once
#include "Building.h"


// BuildingFactory.h
class BuildingFactory {
public:
    static std::unique_ptr<Building> createBuilding(BuildingType type, const glm::vec3& position) {
        switch (type) {
        case BuildingType::RESIDENTIAL:
            return std::make_unique<ResidentialBuilding>(position);
        case BuildingType::COMMERCIAL:
            return std::make_unique<CommercialBuilding>(position);
        case BuildingType::INDUSTRIAL:
            return std::make_unique<IndustrialBuilding>(position);
            // Add more cases as needed
        default:
            return nullptr;
        }
    }
};
