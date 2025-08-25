#pragma once
#include "Chunk.h"
#include <vector>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

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

    void placeBlock(Block block) {
        glm::ivec3 chunkPos = {
            static_cast<int>(std::floor(block.position.x / Chunk::CHUNK_SIZE.x)),
            static_cast<int>(std::floor(block.position.y / Chunk::CHUNK_SIZE.y)),
            static_cast<int>(std::floor(block.position.z / Chunk::CHUNK_SIZE.z))
        };
        Chunk* chunk = getChunkAt(chunkPos);
        if (chunk) {
            glm::ivec3 localPos = static_cast<glm::ivec3>(block.position) - chunkPos * Chunk::CHUNK_SIZE;
            if (localPos.x >= 0 && localPos.x < Chunk::CHUNK_SIZE.x &&
                localPos.y >= 0 && localPos.y < Chunk::CHUNK_SIZE.y &&
                localPos.z >= 0 && localPos.z < Chunk::CHUNK_SIZE.z) {
                chunk->setBlockAt(localPos, block.type);
                chunk->generateMesh();
                chunk->uploadMeshToGPU();
            }
        }
    }
};
