#pragma once
#include <glm/glm.hpp>

class Player {
public:
    glm::vec3 position;
    glm::vec3 direction;

    Player();

    void move(const glm::vec3& delta);
};
