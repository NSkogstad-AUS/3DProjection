#pragma once

class Renderer {
    public:
        void initialise();
        void render();
        void cleanup();
        unsigned int loadShaders(const char* vertexPath, const char* fragmentPath);
};