#include "OBJLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>

bool OBJLoader::loadOBJ(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            Vertex v;
            iss >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        }
        else if (prefix == "vn") {
            Normal n;
            iss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        }
        else if (prefix == "vt") {
            TexCoord t;
            iss >> t.u >> t.v;
            texCoords.push_back(t);
        }
        else if (prefix == "f") {
            parseFace(line.substr(2));
        }
    }

    file.close();
    generateVertexData();
    return true;
}

void OBJLoader::parseFace(const std::string& faceData) {
    std::istringstream iss(faceData);
    std::vector<std::string> vertices;
    std::string vertex;

    // Read all vertices in the face
    while (iss >> vertex) {
        vertices.push_back(vertex);
    }

    if (vertices.size() < 3) {
        std::cerr << "Invalid face with less than 3 vertices" << std::endl;
        return;
    }

    // Triangulate the face
    // For a face with vertices [0, 1, 2, 3, ...], create triangles:
    // [0, 1, 2], [0, 2, 3], [0, 3, 4], etc.
    for (size_t i = 1; i < vertices.size() - 1; ++i) {
        Face face;

        // First vertex (anchor point)
        parseVertexData(vertices[0], face.v1, face.t1, face.n1);

        // Current vertex
        parseVertexData(vertices[i], face.v2, face.t2, face.n2);

        // Next vertex
        parseVertexData(vertices[i + 1], face.v3, face.t3, face.n3);

        faces.push_back(face);
    }
}

void OBJLoader::parseVertexData(const std::string& vertexStr, int& v, int& t, int& n) {
    v = t = n = -1;

    size_t pos1 = vertexStr.find('/');
    if (pos1 == std::string::npos) {
        v = std::stoi(vertexStr) - 1;
    } else {
        v = std::stoi(vertexStr.substr(0, pos1)) - 1;

        size_t pos2 = vertexStr.find('/', pos1 + 1);
        if (pos2 == std::string::npos) {
            std::string texStr = vertexStr.substr(pos1 + 1);
            if (!texStr.empty()) {
                t = std::stoi(texStr) - 1;
            }
        } else {
            std::string texStr = vertexStr.substr(pos1 + 1, pos2 - pos1 - 1);
            if (!texStr.empty()) {
                t = std::stoi(texStr) - 1;
            }

            std::string normalStr = vertexStr.substr(pos2 + 1);
            if (!normalStr.empty()) {
                n = std::stoi(normalStr) - 1;
            }
        }
    }
}

void OBJLoader::generateVertexData() {
    vertexData.clear();

    for (const Face& face : faces) {
        addVertexToData(face.v1, face.t1, face.n1);
        addVertexToData(face.v2, face.t2, face.n2);
        addVertexToData(face.v3, face.t3, face.n3);
    }
}

void OBJLoader::addVertexToData(int vIdx, int tIdx, int nIdx) {
    // Position
    if (vIdx >= 0 && vIdx < vertices.size()) {
        vertexData.push_back(vertices[vIdx].x);
        vertexData.push_back(vertices[vIdx].y);
        vertexData.push_back(vertices[vIdx].z);
    } else {
        vertexData.push_back(0.0f);
        vertexData.push_back(0.0f);
        vertexData.push_back(0.0f);
    }

    // Normal
    if (nIdx >= 0 && nIdx < normals.size()) {
        vertexData.push_back(normals[nIdx].x);
        vertexData.push_back(normals[nIdx].y);
        vertexData.push_back(normals[nIdx].z);
    } else {
        vertexData.push_back(0.0f);
        vertexData.push_back(0.0f);
        vertexData.push_back(1.0f);
    }
}

const std::vector<float>& OBJLoader::getVertexData() const {
    return vertexData;
}

size_t OBJLoader::getVertexCount() const {
    return vertexData.size() / 6;
}

void OBJLoader::printInfo() const {
    std::cout << "OBJ loaded successfully!" << std::endl;
    std::cout << "Vertices: " << vertices.size() << std::endl;
    std::cout << "Normals: " << normals.size() << std::endl;
    std::cout << "Texture Coords: " << texCoords.size() << std::endl;
    std::cout << "Faces: " << faces.size() << std::endl;
    std::cout << "Final vertex count: " << getVertexCount() << std::endl;
}
