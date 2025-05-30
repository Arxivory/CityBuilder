#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include <string>

// Vertex structure
struct Vertex {
    float x, y, z;
};

// Normal structure
struct Normal {
    float x, y, z;
};

// Texture coordinate structure
struct TexCoord {
    float u, v;
};

// Face structure
struct Face {
    int v1, v2, v3;
    int n1, n2, n3;
    int t1, t2, t3;
};

class OBJLoader {
private:
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<TexCoord> texCoords;
    std::vector<Face> faces;
    std::vector<float> vertexData;

    void parseFace(const std::string& faceData);
    void parseVertexData(const std::string& vertexStr, int& v, int& t, int& n);
    void generateVertexData();
    void addVertexToData(int vIdx, int tIdx, int nIdx);

public:
    bool loadOBJ(const std::string& filename);
    const std::vector<float>& getVertexData() const;
    size_t getVertexCount() const;
    void printInfo() const;
};

#endif // OBJLOADER_H
