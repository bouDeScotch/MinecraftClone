#include "../include/Player.h"

Player::Player() {
    position = glm::vec3(0.0f, 1.0f, 3.0f);
}



void Player::placeBlock(World& world, BlockType type, Camera& camera) {
    glm::vec3 rayOrigin = camera.position;
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


void Player::clampVelocity() {
    float horizontalSpeed = glm::length(glm::vec3(velocity.x, 0.0f, velocity.z));
    if (horizontalSpeed > maxSpeed) {
        glm::vec3 horizontalDir = glm::normalize(glm::vec3(velocity.x, 0.0f, velocity.z));
        velocity.x = horizontalDir.x * maxSpeed;
        velocity.z = horizontalDir.z * maxSpeed;
    }
    if (std::abs(velocity.y) > maxVerticalSpeed) {
        velocity.y = (velocity.y > 0 ? 1 : -1) * maxVerticalSpeed;
    }
}

void Player::updatePosition(float deltaTime) {
    position += velocity * deltaTime;
}

void Player::jump(float deltaTime) {
    velocity.y = sqrt(2.0f * jumpHeight * -GRAVITY.y) + GRAVITY.y * deltaTime;
}

void Player::collideWithWorld(World& world) {
    glm::ivec3 blockPos = glm::floor(position);
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            for (int z = -1; z <= 2; z++) {
                glm::ivec3 neighborPos = blockPos + glm::ivec3(x, y, z);
                if (!world.isBlockSolid(neighborPos)) continue;
                glm::vec3 blockMin = glm::vec3(neighborPos);
                glm::vec3 blockMax = blockMin + glm::vec3(1.0f);
                glm::vec3 playerMin = position - glm::vec3(0.5f, 0.0f, 0.5f);
                glm::vec3 playerMax = playerMin + glm::vec3(1.0f, 2.0f, 1.0f);

                if (playerMax.x > blockMin.x && playerMin.x < blockMax.x &&
                    playerMax.y > blockMin.y && playerMin.y < blockMax.y &&
                    playerMax.z > blockMin.z && playerMin.z < blockMax.z) {
                    
                    float overlapX1 = playerMax.x - blockMin.x; // overlap on the right
                    float overlapX2 = blockMax.x - playerMin.x; // overlap on the left
                    float overlapY1 = playerMax.y - blockMin.y; // overlap on the top
                    float overlapY2 = blockMax.y - playerMin.y; // overlap on the bottom
                    float overlapZ1 = playerMax.z - blockMin.z; // overlap on the front
                    float overlapZ2 = blockMax.z - playerMin.z; // overlap on the back

                    float minOverlap = std::min({overlapX1, overlapX2, overlapY1, overlapY2, overlapZ1, overlapZ2});

                    if (minOverlap == overlapX1) {
                        position.x -= overlapX1;
                        velocity.x = 0.0f;
                    } else if (minOverlap == overlapX2) {
                        position.x += overlapX2;
                        velocity.x = 0.0f;
                    } else if (minOverlap == overlapY1) {
                        position.y -= overlapY1;
                        velocity.y = 0.0f;
                    } else if (minOverlap == overlapY2) {
                        position.y += overlapY2;
                        velocity.y = 0.0f;
                    } else if (minOverlap == overlapZ1) {
                        position.z -= overlapZ1;
                        velocity.z = 0.0f;
                    } else if (minOverlap == overlapZ2) {
                        position.z += overlapZ2;
                        velocity.z = 0.0f;
                    }
                }
            }
        }
    }
}

bool Player::isOnGround(World& world) {
    /*  
     float worldY = world.getActualHeightAt(blockPos.x, blockPos.z);
        if (player.position.y <= worldY + 1.01f) { // small epsilon
            player.jump(deltaTime);
        }
        This is the code to check if the player is on the ground
        But we need to run it for the 2 bottom corners of the player to be sure he isn't on a ledge
    */
    float epsilon = 0.01f;
    glm::ivec3 minPos = glm::floor(position - glm::vec3(0.5f, epsilon, 0.5f));
    glm::ivec3 maxPos = glm::floor(position + glm::vec3(0.5f, epsilon, 0.5f));
    float groundY1 = world.getActualHeightAt(minPos.x, minPos.z);
    float groundY2 = world.getActualHeightAt(maxPos.x, maxPos.z);
    return (position.y <= groundY1 + 1.0f + epsilon) || (position.y <= groundY2 + 1.0f + epsilon);
}
