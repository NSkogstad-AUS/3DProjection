#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include <set>

// vertex buffer object
unsigned int VBO, VAO, shaderProgram;

extern Camera camera;

void Renderer::initialise() {
    
    // Define vertices for a 3D cube (counter-clockwise order)
    float vertices[] = {
        // Front face
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,

        // Back face
        -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,

        // Left face
        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,

        // Right face
         0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,

        // Top face
        -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,

        // Bottom face (updated to counter-clockwise order)
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f
    };

    // generating and binding Vertex Array Object (VAO)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // generating and binding the Vertex Buffer Object (VBO)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // defining vertex attribute pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // loading and compiling shaders
    shaderProgram = loadShaders("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
    if (shaderProgram == 0) {
        std::cerr << "Failed to load shaders." << std::endl;
        return;
    }

    // Enabling depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // Default depth test function

    // Enabling face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Verify that depth testing and face culling are enabled
    GLint depthTestEnabled, cullFaceEnabled;
    glGetIntegerv(GL_DEPTH_TEST, &depthTestEnabled);
    glGetIntegerv(GL_CULL_FACE, &cullFaceEnabled);
    std::cout << "Depth Test Enabled: " << (depthTestEnabled ? "Yes" : "No") << std::endl;
    std::cout << "Face Culling Enabled: " << (cullFaceEnabled ? "Yes" : "No") << std::endl;

    // Verify shader program
    if (shaderProgram == 0) {
        std::cerr << "Shader program failed to load." << std::endl;
    } else {
        std::cout << "Shader program loaded successfully." << std::endl;
    }

    // Verify vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
    if (ptr) {
        std::cout << "Vertex data loaded successfully." << std::endl;
        glUnmapBuffer(GL_ARRAY_BUFFER);
    } else {
        std::cerr << "Failed to load vertex data." << std::endl;
    }

    // Unbinding the VBO and VAO to prevent accidental modifications
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Using shader program and then bind the VAO
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    // View matrix from the camera
    glm::mat4 view = camera.GetViewMatrix();

    // Projection matrix: perspective projection
    glm::mat4 project = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);

    // Set the matrices in the shader
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(project));

    // Determine the current chunk based on the camera's x and z positions
    std::pair<int, int> currentChunk = getCurrentChunk(camera.Position.x, camera.Position.z);
    updateVisitedChunks(currentChunk);

    // Drawing the cubes in the visited chunks
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "color");

    for (const auto& chunk : visitedChunks) {
        // Set a unique color for each chunk based on its coordinates
        float outlineColorR = (chunk.first % 2 == 0) ? 1.0f : 0.0f;
        float outlineColorG = (chunk.second % 2 == 0) ? 1.0f : 0.0f;
        float outlineColorB = ((chunk.first + chunk.second) % 2 == 0) ? 1.0f : 0.0f;

        for (int x = chunk.first * CHUNK_SIZE; x < (chunk.first + 1) * CHUNK_SIZE; ++x) {
            for (int y = 0; y < CHUNK_SIZE; ++y) { // Loop over y-axis
                for (int z = chunk.second * CHUNK_SIZE; z < (chunk.second + 1) * CHUNK_SIZE; ++z) {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(x, y, z)); // Include y and z positions

                    // Ensure no negative scaling is applied
                    // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // Example of positive scaling

                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                    // Drawing the cube faces
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glUniform4f(colorLoc, 0.0f, 0.5f, 0.2f, 1.0f);
                    glDrawArrays(GL_TRIANGLES, 0, 36);

                    // Check for OpenGL errors after drawing
                    //checkGLError("After drawing cube faces");

                    // Drawing the wireframe edges with unique color
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glUniform4f(colorLoc, outlineColorR, outlineColorG, outlineColorB, 1.0f);
                    glDrawArrays(GL_TRIANGLES, 0, 36);

                    // Check for OpenGL errors after drawing wireframe
                    //checkGLError("After drawing wireframe");

                    // Resetting the polygon mode
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
            }
        }
    }

    glBindVertexArray(0);
}

void Renderer::updateVisitedChunks(const std::pair<int, int>& chunk) {
    visitedChunks.clear();
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dz = -1; dz <= 1; ++dz) {
            visitedChunks.insert(std::make_pair(chunk.first + dx, chunk.second + dz));
        }
    }
}

std::pair<int, int> Renderer::getCurrentChunk(float cameraX, float cameraZ) {
    int chunkX = static_cast<int>(std::floor(cameraX / CHUNK_SIZE));
    int chunkZ = static_cast<int>(std::floor(cameraZ / CHUNK_SIZE));
    return std::make_pair(chunkX, chunkZ);
}

void Renderer::cleanup() {
    // Good practice to clean up :)
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

unsigned int Renderer::loadShaders(const char* vertexPath, const char* fragmentPath) {
    // Helper function so we can load shaders more neatly
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;

    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);

    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    vShaderFile.close();
    fShaderFile.close();

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // Vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // Fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // Shader program
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // Cleaning up the shaders since they're already linked
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}