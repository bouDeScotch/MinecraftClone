#include "../include/Player.h"

Player::Player() {
    position = glm::vec3(0.0f, 1.0f, 3.0f);
}

void Player::move(const glm::vec3& delta) {
    position += delta;
}

void Player::placeBlock(class World& world, BlockType type) {
    glm::vec3 blockPos = glm::round(position);
    Block block;
    block.position = blockPos;
    block.type = type;
    world.placeBlock(block);
}
