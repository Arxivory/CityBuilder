
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include "OBJLoader.h"
#include "ResidentialBuilding.h"
#include "StraightRoad.h"
#include "ObjectManager.h"
#include "RoadManager.h"
#include "Gizmo.h"
#include "Skybox.h"
#include <map>
#include "Base.h"

glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraYaw = -90.0f, cameraPitch = -20.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool keys[1024];
bool mousePressed = false;
double mouseX, mouseY;

const float M_PI = 3.14159265359f;

glm::vec3 cubePos = glm::vec3(0.0f);
glm::vec3 cubeScale = glm::vec3(0.2f);
glm::vec3 cubeRotation = glm::vec3(0.0f);
bool cubeSelected = false;

ObjectManager objectManager;
RoadManager roadManager;

Gizmo gizmo;
Skybox skybox;
Base base;
bool dragging = false;

void processInput(GLFWwindow* window) {
    if (!gizmo.isDragging()) {  
        float cameraSpeed = 2.5f * deltaTime;

        if (keys[GLFW_KEY_W])
            cameraPos += cameraSpeed * cameraFront;
        if (keys[GLFW_KEY_S])
            cameraPos -= cameraSpeed * cameraFront;
        if (keys[GLFW_KEY_A])
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (keys[GLFW_KEY_D])
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    if (keys[GLFW_KEY_G]) gizmo.setMode(GizmoMode::TRANSLATE);
    if (keys[GLFW_KEY_R]) gizmo.setMode(GizmoMode::ROTATE);
    if (keys[GLFW_KEY_T]) gizmo.setMode(GizmoMode::SCALE);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    Building* selectedBuilding = objectManager.getSelectedBuilding();
    Road* selectedRoad = roadManager.getSelectedRoad();

    if (gizmo.isDragging() && selectedBuilding && !selectedRoad) {
        gizmo.updateDrag(glm::vec2(xpos, ypos), selectedBuilding);
    }
    else if (gizmo.isDragging() && selectedRoad && !selectedBuilding) {
        gizmo.updateDragRoad(glm::vec2(xpos, ypos), selectedRoad);
    }
    else if (mousePressed) {  
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
            return;  
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; 
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        cameraYaw += xoffset;
        cameraPitch += yoffset;

        if (cameraPitch > 89.0f)
            cameraPitch = 89.0f;
        if (cameraPitch < -89.0f)
            cameraPitch = -89.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        direction.y = sin(glm::radians(cameraPitch));
        direction.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        cameraFront = glm::normalize(direction);
    }
}

glm::vec3 screenToWorldRay(double mouseX, double mouseY, int screenWidth, int screenHeight,
    glm::mat4 view, glm::mat4 projection) {
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight;

    glm::vec4 ray_clip = glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
    glm::vec3 ray_world = glm::vec3(glm::inverse(view) * ray_eye);
    return glm::normalize(ray_world);
}

std::map<string, bool> objectTable = {
                {"building", 0},
                {"road", 0}
};

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {

        if (action == GLFW_PRESS) {
            mousePressed = true;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

            glm::vec3 rayDir = screenToWorldRay(mouseX, mouseY, width, height, view, projection);

            float gizmoSize = 0.3f;
            glm::vec3 xAxisEnd = cubePos + glm::vec3(gizmoSize, 0, 0);
            glm::vec3 yAxisEnd = cubePos + glm::vec3(0, gizmoSize, 0);
            glm::vec3 zAxisEnd = cubePos + glm::vec3(0, 0, gizmoSize);

            float pickRadius = 0.1f;

            Building* selectedBuilding = objectManager.getSelectedBuilding();
            Road* selectedRoad = roadManager.getSelectedRoad();

            glm::vec3 gizmoPos = selectedBuilding ? selectedBuilding->getPosition() :
                (selectedRoad ? selectedRoad->getPosition() : glm::vec3(0.0f));
            GizmoAxis selectedAxis = gizmo.checkAxisSelection(cameraPos, rayDir, gizmoPos);
            
            if (selectedAxis == GizmoAxis::NONE) {
                Building* clickedBuilding = objectManager.checkBuildingSelection(cameraPos, rayDir);
                Road* clickedRoad = roadManager.checkRoadSelection(cameraPos, rayDir);
                
                if (clickedBuilding) {
                    objectManager.selectBuilding(cameraPos, rayDir);
                    cubeSelected = true;
                    objectTable["building"] = 1;
                    objectTable["road"] = 0;
                }
                else if (clickedRoad) {
                    roadManager.selectRoad(cameraPos, rayDir);
                    cubeSelected = true;
                    objectTable["building"] = 0;
                    objectTable["road"] = 1;
                }
                else {
                    objectManager.clearSelection();
                    roadManager.clearSelection();
                    cubeSelected = false;
                    objectTable["building"] = 0;
                    objectTable["road"] = 0;
                }
            }
            
            if (selectedAxis != GizmoAxis::NONE && objectTable["building"] == 1) {
                gizmo.startDrag(selectedAxis, glm::vec2(mouseX, mouseY), selectedBuilding);
            }
            else if (selectedAxis != GizmoAxis::NONE && objectTable["road"] == 1) {
                gizmo.startDragRoad(selectedAxis, glm::vec2(mouseX, mouseY), selectedRoad);
            }
            
        }
        else if (action == GLFW_RELEASE) {
            gizmo.endDrag();
            mousePressed = false;
            firstMouse = true;
        }
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1200, 900, "Cube with Transform Gizmos", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    gizmo.initialize();

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(3.0f);

    glm::vec3 lightPos(4.0f, 8.0f, 2.0f);

    skybox.init();
    base.init();
    objectManager.init();
    roadManager.init();

    objectManager.addBuilding(std::make_unique<ResidentialBuilding>(glm::vec3(-2.0f, 0.0f, 0.0f)));
    objectManager.addBuilding(std::make_unique<ResidentialBuilding>(glm::vec3(5.0f, 0.0f, 1.0f)));
    objectManager.addBuilding(std::make_unique<ResidentialBuilding>(glm::vec3(0.0f, 0.0f, 5.0f)));

    roadManager.addRoad(std::make_unique<StraightRoad>(glm::vec3(0.0f, 0.0f, 0.0f)));

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.68f, 0.68f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspect = (float)width / (float)height;

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 8000.0f);

        skybox.render(view, projection);

		base.render(view, projection);
        
        objectManager.renderObjects(view, projection, lightPos, cameraPos);

        roadManager.renderObjects(view, projection, lightPos, cameraPos);


        Building* selectedBuilding = objectManager.getSelectedBuilding();
        Road* selectedRoad = roadManager.getSelectedRoad();

        if (selectedBuilding) {
            gizmo.render(selectedBuilding, view, projection);
        }
        else if (selectedRoad) {
            gizmo.renderRoad(selectedRoad, view, projection); 
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}