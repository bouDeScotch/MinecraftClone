#include "../include/Chunk.h"

Chunk::Chunk() {
    generate();
}

void Chunk::generate() {
    srand(80085); // Fixed seed for reproducibility

    for (float x = 0; x < 16; x++) {
        for (float z = 0; z < 16; z++) {
            // Simple height map using sine waves
            float height = (sin(x / 2.0f) + cos(z / 2.0f)) * 2.0f + 8.0f;
            for (float y = 0; y < height; y++) {
                // Place random BlockType for demonstration
                BlockType type;
                type = static_cast<BlockType>(rand() % 6 + 1);
                blocks.push_back({glm::vec3(x, y, z), type});
            }
        }
    }
}
