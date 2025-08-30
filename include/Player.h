#pragma once
#include <glm/glm.hpp>
#include "Chunk.h"
#include "World.h"
#include "Camera.h"

class Player { 
public:

    static constexpr glm::vec3 GRAVITY = glm::vec3(0.0f, -9.81f, 0.0f);
    static constexpr float LOW_VELOCITY_THRESHOLD = 0.5f;

    float maxSpeed = 10.0f;
    float maxVerticalSpeed = 50.0f;
    float jumpHeight = 1.3f;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 velocity{0.0f};

    Player();

    void placeBlock(class World& world, BlockType type, Camera& camera);
    void breakBlock(class World& world, Camera& camera);
    void clampVelocity();

    void jump(float deltaTime);
    void updatePosition(float deltaTime);
    void collideWithWorld(World& world);

    bool isOnGround(World& world);
};
