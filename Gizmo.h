#pragma once
#ifndef GIZMO_H
#define GIZMO_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "ShaderProgramCreator.h"  // Assuming you have this
#include "Building.h"  // Your base class
#include "Road.h"

enum class GizmoMode { TRANSLATE, ROTATE, SCALE };
enum class GizmoAxis { NONE = -1, X_AXIS = 0, Y_AXIS = 1, Z_AXIS = 2 };

class Gizmo {
private:
    // OpenGL objects
    unsigned int lineVAO, sphereVAO, sphereIndexCount;
    unsigned int shaderProgram;
    ShaderProgramCreator shaderCreator;

    // Gizmo state
    GizmoMode currentMode;
    GizmoAxis activeAxis;
    bool dragging;
    glm::vec2 dragStart;
    glm::vec3 transformStart;
    bool initialized;

    // Private methods
    void createGizmoGeometry();
    void setupShaderProgram();
    void renderAxes(const glm::vec3& position, const glm::mat4& view, const glm::mat4& projection);
    void renderAxisSpheres(const glm::vec3& position, const glm::mat4& view, const glm::mat4& projection);

public:
    Gizmo();
    ~Gizmo();

    void initialize();
    void render(Building* selectedBuilding, const glm::mat4& view, const glm::mat4& projection);
    void renderRoad(Road* selectedRoad, const glm::mat4& view, const glm::mat4& projection);
    GizmoAxis checkAxisSelection(const glm::vec3& rayStart, const glm::vec3& rayDir, const glm::vec3& objectPos);

    void startDrag(GizmoAxis axis, const glm::vec2& mousePos, Building* building);
    void startDragRoad(GizmoAxis axis, const glm::vec2& mousePos, Road* road);
    void updateDrag(const glm::vec2& currentMousePos, Building* building);
    void updateDragRoad(const glm::vec2& currentMousePos, Road* road);
    void endDrag();

    // Mode control
    void setMode(GizmoMode mode) { currentMode = mode; }
    GizmoMode getMode() const { return currentMode; }
    bool isDragging() const { return dragging; }
    GizmoAxis getActiveAxis() const { return activeAxis; }
};

#endif // !GIZMO_H