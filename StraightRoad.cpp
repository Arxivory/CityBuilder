#include "StraightRoad.h"
#include <iostream>

StraightRoad::StraightRoad(const vec3& pos)
	: Road(RoadType::STRAIGHT, "models/StraightRoad.obj"),
	VAO(0), VBO(0), isInitialized(false) {

	position = pos;
	rotation = vec3(0.0f);
	scale = vec3(0.002f);
	selected = false;
}

StraightRoad::~StraightRoad() {
	cleanup();
}

bool StraightRoad::initialize() {
	if (isInitialized) {
		return true;
	}

	if (!objModel.loadOBJ(modelPath)) {
		cerr << "Failed to load residential building model: " << modelPath << std::endl;
		return false;
	}

	setupMesh();
	isInitialized = true;

	cout << "Straight Road initialized successfully" << std::endl;
	objModel.printInfo();

	return true;
}

bool StraightRoad::intersects(const vec3& rayStart, const vec3& rayDir) {
	float radius = 0.8f;
	vec3 oc = rayStart - position;
	float a = glm::dot(rayDir, rayDir);
	float b = 2.0f * glm::dot(oc, rayDir);
	float c = glm::dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a * c;
	return discriminant >= 0;
}

void StraightRoad::setupMesh() {
	// Get vertex data from OBJ loader
	const std::vector<float> vertexData = objModel.getVertexData();

	if (vertexData.empty()) {
		std::cerr << "No vertex data available for residential building" << std::endl;
		return;
	}

	// Generate and bind VAO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	// Bind and fill VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float),
		vertexData.data(), GL_STATIC_DRAW);

	// Position attribute (location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Normal attribute (location = 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Unbind VAO
	glBindVertexArray(0);
}

void StraightRoad::render(unsigned int shaderProgram, const mat4& view,
	const mat4& projection, const vec3& lightPos, const vec3& cameraPos) {
	if (!isInitialized) {
		if (!initialize()) {
			return; // Failed to initialize, can't render
		}
	}

	if (VAO == 0) {
		std::cerr << "VAO not properly initialized for residential building" << std::endl;
		return;
	}

	// Use the shader program
	glUseProgram(shaderProgram);

	// Create model matrix with transformations
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);

	// Apply rotations in order: X, Y, Z
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	model = glm::scale(model, scale);

	// Set uniforms
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE,
		glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));


	glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
	glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
	glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));

	// Set road specific color (brownish/orange for houses)
	glUniform3f(glGetUniformLocation(shaderProgram, "roadColor"), 0.1f, 0.1f, 0.1f);

	// Set selection highlight
	glUniform1i(glGetUniformLocation(shaderProgram, "selected"), selected);

	// Bind VAO and draw
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, objModel.getVertexCount());
	glBindVertexArray(0);
}

void StraightRoad::cleanup() {
	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	isInitialized = false;
}