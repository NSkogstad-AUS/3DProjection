#pragma once
#include <set>
#include <vector>
#include <string>

class Renderer {
public:
    void initialise();
    void render();
    void cleanup();
    std::vector<float> loadHeightMap(const std::string& filePath, int& width, int& height);
    std::vector<float> generateTerrainVertices(const std::vector<float>& heightMap, int width, int height);
    void updateVisitedChunks(const std::pair<int, int>& chunk);
    std::pair<int, int> getCurrentChunk(float cameraX, float cameraZ);
    unsigned int loadShaders(const char* vertexPath, const char* fragmentPath);

private:
    unsigned int cubeVBO, cubeVAO, terrainVBO, terrainVAO, shaderProgram;
    std::vector<float> terrainVertices; // Add this line
    std::set<std::pair<int, int>> visitedChunks;
};