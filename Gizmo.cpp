#include "Gizmo.h"
#include <iostream>
#include <cmath>

const float M_PI = 3.14159265359f;

// Shader sources
const char* gizmoVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* gizmoFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 gizmoColor;

void main() {
    FragColor = vec4(gizmoColor, 1.0);
}
)";

Gizmo::Gizmo()
    : lineVAO(0), sphereVAO(0), sphereIndexCount(0), shaderProgram(0),
    currentMode(GizmoMode::TRANSLATE), activeAxis(GizmoAxis::NONE),
    dragging(false), dragStart(0.0f), transformStart(0.0f), initialized(false) {
}

Gizmo::~Gizmo() {
    if (initialized) {
        glDeleteVertexArrays(1, &lineVAO);
        glDeleteVertexArrays(1, &sphereVAO);
        glDeleteProgram(shaderProgram);
    }
}

void Gizmo::initialize() {
    if (initialized) return;

    createGizmoGeometry();
    setupShaderProgram();
    initialized = true;
}

void Gizmo::createGizmoGeometry() {
    // Line geometry for axes
    float lineVertices[] = {
        // X axis (red)
        0.0f, 0.0f, 0.0f,
        0.3f, 0.0f, 0.0f,
        // Y axis (green)
        0.0f, 0.0f, 0.0f,
        0.0f, 0.3f, 0.0f,
        // Z axis (blue)
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.3f
    };

    unsigned int lineVBO;
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Simple sphere for axis endpoints
    std::vector<float> sphereVertices;
    std::vector<unsigned int> sphereIndices;

    const int rings = 8;
    const int sectors = 8;
    const float radius = 0.05f;

    for (int i = 0; i <= rings; ++i) {
        float phi = M_PI * float(i) / float(rings);
        for (int j = 0; j <= sectors; ++j) {
            float theta = 2.0f * M_PI * float(j) / float(sectors);

            float x = radius * sinf(phi) * cosf(theta);
            float y = radius * cosf(phi);
            float z = radius * sinf(phi) * sinf(theta);

            sphereVertices.push_back(x);
            sphereVertices.push_back(y);
            sphereVertices.push_back(z);
        }
    }

    for (int i = 0; i < rings; ++i) {
        for (int j = 0; j < sectors; ++j) {
            int first = i * (sectors + 1) + j;
            int second = first + sectors + 1;

            sphereIndices.push_back(first);
            sphereIndices.push_back(second);
            sphereIndices.push_back(first + 1);

            sphereIndices.push_back(second);
            sphereIndices.push_back(second + 1);
            sphereIndices.push_back(first + 1);
        }
    }

    sphereIndexCount = sphereIndices.size();

    unsigned int sphereVBO, sphereEBO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Gizmo::setupShaderProgram() {
    // You can use your ShaderProgramCreator here, or compile manually
    // Using manual compilation for compatibility with your existing code

    auto compileShader = [](unsigned int type, const char* source) -> unsigned int {
        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);

        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cout << "Gizmo shader compilation failed: " << infoLog << std::endl;
        }
        return shader;
        };

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, gizmoVertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, gizmoFragmentShaderSource);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check linking
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Gizmo shader program linking failed: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Gizmo::render(Building* selectedBuilding, const glm::mat4& view, const glm::mat4& projection) {
    if (!selectedBuilding || !initialized) return;

    glm::vec3 position = selectedBuilding->getPosition();

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    renderAxes(position, view, projection);
    renderAxisSpheres(position, view, projection);
}

void Gizmo::renderRoad(Road* selectedRoad, const glm::mat4& view, const glm::mat4& projection) {
    if (!selectedRoad || !initialized) return;

    glm::vec3 position = selectedRoad->getPosition();

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    renderAxes(position, view, projection);
    renderAxisSpheres(position, view, projection);
}

void Gizmo::renderAxes(const glm::vec3& position, const glm::mat4& view, const glm::mat4& projection) {
    // Gizmo at object position
    glm::mat4 gizmoModel = glm::translate(glm::mat4(1.0f), position);
    gizmoModel = glm::scale(gizmoModel, glm::vec3(8.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(gizmoModel));

    // Draw axis lines
    glBindVertexArray(lineVAO);

    // X axis (red)
    glUniform3f(glGetUniformLocation(shaderProgram, "gizmoColor"),
        activeAxis == GizmoAxis::X_AXIS ? 1.0f : 0.8f, 0.0f, 0.0f);
    glDrawArrays(GL_LINES, 0, 2);

    // Y axis (green)
    glUniform3f(glGetUniformLocation(shaderProgram, "gizmoColor"),
        0.0f, activeAxis == GizmoAxis::Y_AXIS ? 1.0f : 0.8f, 0.0f);
    glDrawArrays(GL_LINES, 2, 2);

    // Z axis (blue)
    glUniform3f(glGetUniformLocation(shaderProgram, "gizmoColor"),
        0.0f, 0.0f, activeAxis == GizmoAxis::Z_AXIS ? 1.0f : 0.8f);
    glDrawArrays(GL_LINES, 4, 2);
}

void Gizmo::renderAxisSpheres(const glm::vec3& position, const glm::mat4& view, const glm::mat4& projection) {
    glm::mat4 gizmoModel = glm::translate(glm::mat4(1.0f), position);
    gizmoModel = glm::scale(gizmoModel, glm::vec3(3.0f));

    // Draw axis endpoint spheres
    glBindVertexArray(sphereVAO);

    // X axis sphere (red)
    glm::mat4 xSphere = glm::translate(gizmoModel, glm::vec3(0.8f, 0.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(xSphere));
    glUniform3f(glGetUniformLocation(shaderProgram, "gizmoColor"),
        activeAxis == GizmoAxis::X_AXIS ? 1.0f : 0.8f, 0.0f, 0.0f);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);

    // Y axis sphere (green)
    glm::mat4 ySphere = glm::translate(gizmoModel, glm::vec3(0.0f, 0.8f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(ySphere));
    glUniform3f(glGetUniformLocation(shaderProgram, "gizmoColor"),
        0.0f, activeAxis == GizmoAxis::Y_AXIS ? 1.0f : 0.8f, 0.0f);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);

    // Z axis sphere (blue)
    glm::mat4 zSphere = glm::translate(gizmoModel, glm::vec3(0.0f, 0.0f, 0.8f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(zSphere));
    glUniform3f(glGetUniformLocation(shaderProgram, "gizmoColor"),
        0.0f, 0.0f, activeAxis == GizmoAxis::Z_AXIS ? 1.0f : 0.8f);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
}

// Ray-sphere intersection for gizmo picking
bool raySphereIntersect(const glm::vec3& rayStart, const glm::vec3& rayDir, const glm::vec3& sphereCenter, float radius) {
    glm::vec3 oc = rayStart - sphereCenter;
    float a = glm::dot(rayDir, rayDir);
    float b = 2.0f * glm::dot(oc, rayDir);
    float c = glm::dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    return discriminant >= 0;
}

GizmoAxis Gizmo::checkAxisSelection(const glm::vec3& rayStart, const glm::vec3& rayDir, const glm::vec3& objectPos) {
    if (!initialized) return GizmoAxis::NONE;

    // Check gizmo axis selection (simplified - checking spheres at axis endpoints)
    float gizmoSize = 0.8f * 3.0f; // Scale factor applied
    glm::vec3 xAxisEnd = objectPos + glm::vec3(gizmoSize, 0, 0);
    glm::vec3 yAxisEnd = objectPos + glm::vec3(0, gizmoSize, 0);
    glm::vec3 zAxisEnd = objectPos + glm::vec3(0, 0, gizmoSize);

    float pickRadius = 0.3f; // Larger picking radius for easier selection

    if (raySphereIntersect(rayStart, rayDir, xAxisEnd, pickRadius)) {
        return GizmoAxis::X_AXIS;
    }
    else if (raySphereIntersect(rayStart, rayDir, yAxisEnd, pickRadius)) {
        return GizmoAxis::Y_AXIS;
    }
    else if (raySphereIntersect(rayStart, rayDir, zAxisEnd, pickRadius)) {
        return GizmoAxis::Z_AXIS;
    }

    return GizmoAxis::NONE;
}

void Gizmo::startDrag(GizmoAxis axis, const glm::vec2& mousePos, Building* building) {
    activeAxis = axis;
    dragging = true;
    dragStart = mousePos;

    if (building) {
        if (currentMode == GizmoMode::TRANSLATE) {
            transformStart = building->getPosition();
        }
        else if (currentMode == GizmoMode::ROTATE) {
            transformStart = building->getRotation();
        }
        else if (currentMode == GizmoMode::SCALE) {
            transformStart = building->getScale();
        }
    }
}

void Gizmo::updateDrag(const glm::vec2& currentMousePos, Building* building) {
    if (!dragging || activeAxis == GizmoAxis::NONE || !building) return;

    glm::vec2 delta = currentMousePos - dragStart;
    float sensitivity = 0.01f;

    if (currentMode == GizmoMode::TRANSLATE) {
        glm::vec3 newPos = transformStart;
        if (activeAxis == GizmoAxis::X_AXIS) {
            newPos.x = transformStart.x + delta.x * sensitivity;
        }
        else if (activeAxis == GizmoAxis::Y_AXIS) {
            newPos.y = transformStart.y - delta.y * sensitivity;
        }
        else if (activeAxis == GizmoAxis::Z_AXIS) {
            newPos.z = transformStart.z + delta.x * sensitivity;
        }
        building->setPosition(newPos);
    }
    else if (currentMode == GizmoMode::ROTATE) {
        float rotSensitivity = 2.0f;
        glm::vec3 newRot = transformStart;
        if (activeAxis == GizmoAxis::X_AXIS) {
            newRot.x = transformStart.x + delta.y * rotSensitivity;
        }
        else if (activeAxis == GizmoAxis::Y_AXIS) {
            newRot.y = transformStart.y + delta.x * rotSensitivity;
        }
        else if (activeAxis == GizmoAxis::Z_AXIS) {
            newRot.z = transformStart.z + delta.x * rotSensitivity;
        }
        building->setRotation(newRot);
    }
    else if (currentMode == GizmoMode::SCALE) {
        float scaleSensitivity = 0.01f;
        float scaleChange = delta.x * scaleSensitivity;
        glm::vec3 newScale = transformStart;
        if (activeAxis == GizmoAxis::X_AXIS) {
            newScale.x = glm::max(0.1f, transformStart.x + scaleChange);
        }
        else if (activeAxis == GizmoAxis::Y_AXIS) {
            newScale.y = glm::max(0.1f, transformStart.y - delta.y * scaleSensitivity);
        }
        else if (activeAxis == GizmoAxis::Z_AXIS) {
            newScale.z = glm::max(0.1f, transformStart.z + scaleChange);
        }
        building->setScale(newScale);
    }
}



void Gizmo::startDragRoad(GizmoAxis axis, const glm::vec2& mousePos, Road* road) {
    activeAxis = axis;
    dragging = true;
    dragStart = mousePos;

    if (road) {
        if (currentMode == GizmoMode::TRANSLATE) {
            transformStart = road->getPosition();
        }
        else if (currentMode == GizmoMode::ROTATE) {
            transformStart = road->getRotation();
        }
        else if (currentMode == GizmoMode::SCALE) {
            transformStart = road->getScale();
        }
    }
}

void Gizmo::updateDragRoad(const glm::vec2& currentMousePos, Road* road) {
    if (!dragging || activeAxis == GizmoAxis::NONE || !road) return;

    glm::vec2 delta = currentMousePos - dragStart;
    float sensitivity = 0.01f;

    if (currentMode == GizmoMode::TRANSLATE) {
        glm::vec3 newPos = transformStart;
        if (activeAxis == GizmoAxis::X_AXIS) {
            newPos.x = transformStart.x + delta.x * sensitivity;
        }
        else if (activeAxis == GizmoAxis::Y_AXIS) {
            newPos.y = transformStart.y - delta.y * sensitivity;
        }
        else if (activeAxis == GizmoAxis::Z_AXIS) {
            newPos.z = transformStart.z + delta.x * sensitivity;
        }
        road->setPosition(newPos);
    }
    else if (currentMode == GizmoMode::ROTATE) {
        float rotSensitivity = 2.0f;
        glm::vec3 newRot = transformStart;
        if (activeAxis == GizmoAxis::X_AXIS) {
            newRot.x = transformStart.x + delta.y * rotSensitivity;
        }
        else if (activeAxis == GizmoAxis::Y_AXIS) {
            newRot.y = transformStart.y + delta.x * rotSensitivity;
        }
        else if (activeAxis == GizmoAxis::Z_AXIS) {
            newRot.z = transformStart.z + delta.x * rotSensitivity;
        }
        road->setRotation(newRot);
    }
    else if (currentMode == GizmoMode::SCALE) {
        float scaleSensitivity = 0.0001f;
        float scaleChange = delta.x * scaleSensitivity;
        glm::vec3 newScale = transformStart;
        if (activeAxis == GizmoAxis::X_AXIS) {
            newScale.x = glm::max(0.002f, transformStart.x + scaleChange);
        }
        else if (activeAxis == GizmoAxis::Y_AXIS) {
            newScale.y = glm::max(0.002f, transformStart.y - delta.y * scaleSensitivity);
        }
        else if (activeAxis == GizmoAxis::Z_AXIS) {
            newScale.z = glm::max(0.002f, transformStart.z + scaleChange);
        }
        road->setScale(newScale);
    }
}

void Gizmo::endDrag() {
    dragging = false;
    activeAxis = GizmoAxis::NONE;
}