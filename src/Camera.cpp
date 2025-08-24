#include "../include/Camera.h"

Camera::Camera() {
    position = glm::vec3(0.0f, 1.0f, 3.0f);
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
}
