#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "renderer.h"

int main() {
    // Initalising GLFW
    if(!glfwInit()) {
        std::cerr << "Failed to initalise GLFW" << std::endl;
        return -1;
    }

    // Setting OpenGL version to 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Needed for MacOS
    #endif

    // creating the window
    GLFWwindow* window = glfwCreateWindow(800, 600, "3D Render", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // making context current
    glfwMakeContextCurrent(window);

    // glewExperimental = GL_TRUE; // Uncomment if using GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // set our viewport
    glViewport(0, 0, 800, 600);

    // Create and initalise the renderer
    Renderer renderer;
    renderer.initialise();

    // Main rendering loop
    while (!glfwWindowShouldClose(window)) {
        // Checking events
        glfwPollEvents();

        // Rendering scene
        renderer.render();

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // And cleanup
    renderer.cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}