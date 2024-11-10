#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    glm::vec3 currentSelected;

    // Euler Angles
    float Yaw;
    float Pitch;

    // Camera options
    float Speed;
    float Sensitivity;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), 
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
           float yaw = -90.0f, 
           float pitch = 0.0f, 
           float speed = 2.5f, 
           float sensitivity = 0.1f)
        : Position(position), WorldUp(up), Yaw(yaw), Pitch(pitch), Speed(speed), Sensitivity(sensitivity) 
    {
        Front = glm::vec3(0.0f, 0.0f, -1.0f);
        updateCameraVectors();
    }

    // Process keyboard input to move the camera
    void ProcessKeyboard(GLFWwindow* window, float deltaTime) {
        float velocity = Speed * deltaTime;
        glm::vec3 direction(0.0f);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            direction += Front;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            direction -= Front;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            direction -= Right;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            direction += Right;

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            velocity *= 2.0f; // Increase speed when left shift is pressed

        Position += direction * velocity;
    }

    // Process mouse movement to look around
    void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true) {
        xOffset *= Sensitivity;
        yOffset *= Sensitivity;

        Yaw += xOffset;
        Pitch += yOffset;

        // Constrain the pitch to prevent flipping
        if (constrainPitch) {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // Get the view matrix using glm::lookAt
    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

private:
    // Updates the camera's Front, Right, and Up vectors based on the updated Euler angles
    void updateCameraVectors() {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        // Re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};

#endif
