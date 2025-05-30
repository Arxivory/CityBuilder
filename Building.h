#pragma once
#ifndef BUILDING_H
#define BUILDING_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "BuildingTypes.h"

using namespace glm;
using namespace std;

class Building {
protected:
    BuildingType type;
    string modelPath;

public:
    vec3 position;
    vec3 rotation;
    vec3 scale;
    bool selected;

    // Constructor declaration only
    Building(BuildingType buildingType, const string& objPath);

    // Virtual destructor declaration
    virtual ~Building();

    // Member function declarations only
    BuildingType getType() const;
    const string& getModelPath() const;

    bool virtual intersects(const glm::vec3& rayStart, const glm::vec3& rayDir) = 0;

    // Pure virtual function
    virtual void render(unsigned int shaderProgram, const mat4& view,
        const mat4& projection, const vec3& lightPos, const vec3& cameraPos) = 0;

    virtual glm::vec3 getPosition() { return position; }
    virtual void setPosition(const glm::vec3& pos) { position = pos; }
    virtual glm::vec3 getRotation() { return rotation; }
    virtual void setRotation(const glm::vec3& rot) { rotation = rot;  }
    virtual glm::vec3 getScale() { return scale; }
    virtual void setScale(const glm::vec3& scl) { scale = scl; }

};

#endif // BUILDING_H