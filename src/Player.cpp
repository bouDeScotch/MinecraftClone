#include "../include/Player.h"

Player::Player() {
    position = glm::vec3(0.0f, 1.0f, 3.0f);
    direction = glm::vec3(0.0f, 0.0f, -1.0f);
}

void Player::move(const glm::vec3& delta) {
    position += delta;
}
