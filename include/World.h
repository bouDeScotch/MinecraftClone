#pragma once
#include "Chunk.h"
#include <vector>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <unordered_map>

class World {
public:
    struct IVec3Hash {
        size_t operator()(const glm::ivec3& v) const {
            return ((std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1)) >> 1) ^ (std::hash<int>()(v.z) << 1);
        }
    };

    std::unordered_map<glm::ivec3, Chunk, IVec3Hash> chunkMap;    
    World() {}

    void createChunkAt(const glm::ivec3& pos) {
        if (chunkMap.find(pos) == chunkMap.end()) {
            Chunk chunk(pos);
            chunk.generate();
            chunkMap.emplace(pos, chunk);
        }
    }

    void generateChunks(int radius, glm::ivec3 centerChunk) {
        for (int x = -radius; x <= radius; x++) {
            for (int z = -radius; z <= radius; z++) {
                glm::ivec3 chunkPos = centerChunk + glm::ivec3(x, 0, z);
                createChunkAt(chunkPos);
            }
        }
        std::cout << "Generated " << chunkMap.size() << " chunks.\n";
    }

    Chunk* getChunkAt(const glm::ivec3& pos) {
        auto it = chunkMap.find(pos);
        if (it != chunkMap.end()) return &it->second;
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

    std::vector<glm::ivec3> getAllChunksToDraw(glm::ivec3 playerChunkPos, int viewDistance) {
        std::vector<glm::ivec3> chunksToDraw;
        for (int x = -viewDistance; x <= viewDistance; x++) {
            for (int z = -viewDistance; z <= viewDistance; z++) {
                glm::ivec3 chunkPos = playerChunkPos + glm::ivec3(x, 0, z);
                chunkPos.y = 0; // For now, only ground level
                chunksToDraw.push_back(chunkPos);
            }
        }
        return chunksToDraw;
    }

};
