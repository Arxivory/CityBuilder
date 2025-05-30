#pragma once
#ifndef RESIDENTIAL_BUILDING_H
#define RESIDENTIAL_BUILDING_H

#include "Building.h"
#include "OBJLoader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ResidentialBuilding : public Building {
private:
    OBJLoader objModel;
    unsigned int VAO, VBO;
    bool isInitialized;
    
    void setupMesh();
    void cleanup();

public:
    ResidentialBuilding(const glm::vec3& pos);
    ~ResidentialBuilding();
    
    bool initialize();

    bool intersects(const glm::vec3& rayStart, const glm::vec3& rayDir) override;

    void render(unsigned int shaderProgram, const glm::mat4& view, 
               const glm::mat4& projection, const vec3& lightPos, const vec3& cameraPos) override;
};

#endif
