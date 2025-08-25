#pragma once
#include "Chunk.h"
#include <vector>
#include <iostream>

class World {
public:
    std::vector<Chunk> chunks;

    World() {}

    void generateChunks(int radius, glm::ivec3 centerChunk) {
        for (int x = -radius; x <= radius; x++) {
            for (int z = -radius; z <= radius; z++) {
                glm::ivec3 chunkPos = centerChunk + glm::ivec3(x, 0, z);
                chunks.emplace_back(chunkPos);
            }
        }
        std::cout << "Generated " << chunks.size() << " chunks.\n";
    }

    Chunk* getChunkAt(const glm::ivec3& position) {
        for (auto& chunk : chunks) {
            if (chunk.chunkPos == position) {
                return &chunk;
            }
        }
        return nullptr;
    }
};
