#pragma once
#include <set>

class Renderer {
    public:
        void initialise();
        void render();
        void cleanup();
        unsigned int loadShaders(const char* vertexPath, const char* fragmentPath);
    private:
        static const int CHUNK_SIZE = 10;
        std::pair<int, int> getCurrentChunk(float cameraX, float cameraZ);
        void updateVisitedChunks(const std::pair<int, int>& chunk);

        std::set<std::pair<int, int>> visitedChunks;
};