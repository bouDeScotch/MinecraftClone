#pragma once
#include <glm/glm.hpp>
#include "Chunk.h"
#include "World.h"
#include "Camera.h"

class Player {
public:
    glm::vec3 position;
    glm::vec3 direction;

    Player();

    void move(const glm::vec3& delta);

    void placeBlock(class World& world, BlockType type, Camera& camera);
};
