#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;

    float yaw;   // rotation horizontale
    float pitch; // rotation verticale

    Camera(glm::vec3 pos = glm::vec3(0.0f,0.0f,3.0f)) 
        : position(pos), front(glm::vec3(0.0f,0.0f,-1.0f)), 
          up(glm::vec3(0.0f,1.0f,0.0f)), yaw(-90.0f), pitch(0.0f) {}

    void processMouseMovement(float xoffset, float yoffset) {
        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if(pitch > 89.0f) pitch = 89.0f;
        if(pitch < -89.0f) pitch = -89.0f;

        glm::vec3 f;
        f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        f.y = sin(glm::radians(pitch));
        f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(f);
    }
};
