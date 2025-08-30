#include "../include/Player.h"

Player::Player() {
    position = glm::vec3(0.0f, 1.0f, 3.0f);
}

void Player::move(const glm::vec3& delta) {
    position += delta;
}



void Player::placeBlock(World& world, BlockType type, Camera& camera) {
    glm::vec3 rayOrigin = position;
    glm::vec3 rayDir = glm::normalize(camera.front);

    glm::ivec3 current = glm::floor(rayOrigin);
    glm::vec3 tMax;
    glm::vec3 tDelta;
    glm::ivec3 step;

    // Calcul des steps et tMax/tDelta par axe
    for (int i = 0; i < 3; ++i) {
        if (rayDir[i] > 0) {
            step[i] = 1;
            tMax[i] = (current[i] + 1.0f - rayOrigin[i]) / rayDir[i];
        } else if (rayDir[i] < 0) {
            step[i] = -1;
            tMax[i] = (current[i] - rayOrigin[i]) / rayDir[i];
        } else {
            step[i] = 0;
            tMax[i] = std::numeric_limits<float>::infinity();
        }
        tDelta[i] = (step[i] != 0) ? 1.0f / std::abs(rayDir[i]) : std::numeric_limits<float>::infinity();
    }

    const float maxDistance = 10.0f;
    float traveled = 0.0f;

    glm::ivec3 lastEmpty = current;

    while (traveled < maxDistance) {
        if (world.isBlockSolid(current)) {
            // place devant le bloc touché
            Block block;
            block.position = lastEmpty;
            block.type = type;
            world.placeBlock(block);
            // Generate mesh for affected chunk
            glm::ivec3 chunkPos = {
                static_cast<int>(std::floor(block.position.x / Chunk::CHUNK_SIZE.x)),
                static_cast<int>(std::floor(block.position.y / Chunk::CHUNK_SIZE.y)),
                static_cast<int>(std::floor(block.position.z / Chunk::CHUNK_SIZE.z))
            };
            return;
        }

        lastEmpty = current;

        // avancer vers la face du voxel la plus proche
        if (tMax.x < tMax.y && tMax.x < tMax.z) {
            traveled = tMax.x;
            tMax.x += tDelta.x;
            current.x += step.x;
        } else if (tMax.y < tMax.z) {
            traveled = tMax.y;
            tMax.y += tDelta.y;
            current.y += step.y;
        } else {
            traveled = tMax.z;
            tMax.z += tDelta.z;
            current.z += step.z;
        }
    }
}


