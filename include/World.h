#pragma once
#include "Chunk.h"
#include <memory>
#include <vector>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <unordered_map>
#include "PerlinNoise.hpp"
#include <map>
#include <time.h>

struct Structure {
    std::vector<BlockType> types;
    std::vector<glm::ivec3> positions; // positions
    std::vector<float> probabilities; // probabilities for each block
};

class World {
public:

    // Move seed here
    siv::PerlinNoise::seed_type seed;
    siv::PerlinNoise perlin;

    std::string chunkDir;

    std::map<std::string, Structure> structures = { 
        
        {"tree",
         {
            // types
            {
                // Tronc
                WOOD, WOOD, WOOD, WOOD, WOOD, WOOD,
                // Feuillage couche 1
                LEAF, LEAF, LEAF, LEAF, LEAF, LEAF, LEAF, LEAF, LEAF,
                // Feuillage couche 2
                LEAF, LEAF, LEAF, LEAF, LEAF,
                // Feuillage sommet
                LEAF
            },
            // positions
            {
                // Tronc
                {0,0,0}, {0,1,0}, {0,2,0}, {0,3,0}, {0,4,0}, {0,5,0},
                // Feuilles couche 1 (3x3 autour du tronc à y=4)
                {-1,4,-1}, {0,4,-1}, {1,4,-1},
                {-1,4, 0}, {0,4, 0}, {1,4, 0},
                {-1,4, 1}, {0,4, 1}, {1,4, 1},
                // Feuilles couche 2 (2x2 à y=5)
                {-1,5,0}, {0,5,0}, {0,5,-1}, {1,5,-1}, {1,5,0},
                // Feuille sommet
                {0,6,0}
            },
            // probabilités pour chaque bloc
            {
                // Tronc (toujours posé)
                1,1,1,1,1,1,
                // Feuilles couche 1 (densité 80%)
                0.8f,0.8f,0.8f,0.8f,0.8f,0.8f,0.8f,0.8f,0.8f,
                // Feuilles couche 2 (densité 60%)
                0.6f,0.6f,0.6f,0.6f,0.6f,
                // Feuille sommet (toujours)
                1.0f
            }
         }
        }


    };

    struct IVec3Hash {
        size_t operator()(const glm::ivec3& v) const {
            return ((std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1)) >> 1) ^ (std::hash<int>()(v.z) << 1);
        }
    };

    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>, IVec3Hash> chunkMap;    
    World() {
        // Create directory for chunks if it doesn't exist
        std::string dir = "../chunks/" + std::to_string(seed);
        if (system(("mkdir -p " + dir).c_str()) != 0) {
            std::cerr << "Failed to create directory: " << dir << std::endl;
        }

        chunkDir = "../chunks/" + std::to_string(seed) + "/";
        seed = static_cast<siv::PerlinNoise::seed_type>(time(NULL));
        perlin = siv::PerlinNoise(seed);
    }

    std::string getFilenameForChunk(const glm::ivec3& pos) {
        long long index = (static_cast<long long>(pos.x) & 0xFFFFF) << 40 |
                         (static_cast<long long>(pos.y) & 0xFFFFF) << 20 |
                         (static_cast<long long>(pos.z) & 0xFFFFF);
        return chunkDir + "chunk_" + std::to_string(index);
    }

    
    void createChunkAt(const glm::ivec3& pos) {
        if (chunkMap.find(pos) == chunkMap.end()) {
            auto filename = getFilenameForChunk(pos);

            std::shared_ptr<Chunk> chunkPtr;

            // Si le chunk existe dans un fichier, on le charge
            if (Chunk::isInFile(filename)) {
                chunkPtr = std::make_shared<Chunk>(pos); // construit directement
                chunkPtr->loadFromFile(filename);
                std::cout << "Loaded chunk from file: " << filename << std::endl;
            } else {
                chunkPtr = std::make_shared<Chunk>(pos); // construit directement
                chunkPtr->generate(perlin);
                chunkPtr->saveToFile(filename);
                std::cout << "Generated and saved chunk at " << glm::to_string(pos);
            }

            chunkMap[pos] = chunkPtr;
        }
    }


    void placeStructure(const std::string& name, const glm::ivec3& basePos) {
        auto it = structures.find(name);
        if (it == structures.end()) {
            std::cerr << "Structure not found: " << name << std::endl;
            return;
        }
        const Structure& structure = it->second;
        for (size_t i = 0; i < structure.types.size(); ++i) {
            float prob = (i < structure.probabilities.size()) ? structure.probabilities[i] : 1.0f;
            float noiseVal = perlin.octave3D_01((basePos.x + structure.positions[i].x) * 0.1f,
                                                  (basePos.y + structure.positions[i].y) * 0.1f,
                                                  (basePos.z + structure.positions[i].z) * 0.1f, 4);
            if (noiseVal > prob) continue; // skip this block based on probability
            BlockType type = structure.types[i];
            glm::ivec3 offset = structure.positions[i];
            Block block;
            block.type = type;
            block.position = glm::vec3(basePos + offset);
            placeBlock(block);
        }
    }


    void generateStructureInChunk(const glm::ivec3& chunkPos) {
        // Generate trees
        // Use perlin noise to decide if we place a tree
        for (int x = 0; x < Chunk::CHUNK_SIZE.x; x++) {
            int worldX = x + chunkPos.x * Chunk::CHUNK_SIZE.x;
            for (int z = 0; z < Chunk::CHUNK_SIZE.z; z++) {
                int worldZ = z + chunkPos.z * Chunk::CHUNK_SIZE.z;
                float treeChance = perlin.octave2D_01(worldX * 0.1f, worldZ * 0.1f, 4);
                if (treeChance > 0.8f) {
                    // Check ground block type
                    int height = getHeightAt(worldX, worldZ);
                    glm::ivec3 treeBasePos = {worldX, height, worldZ};
                    if (!isBlockSolid(treeBasePos)) continue;
                    Block belowBlock = getBlockAt(treeBasePos); // Structure is relative to ground so no need to subtract 1
                    if (belowBlock.type != GRASS && belowBlock.type != DIRT) continue;
                    placeStructure("tree", treeBasePos);
                    z += 3; // éviter de placer des arbres trop proches
                    x += 3;
                }
            }
        }
    }

    int getHeightAt(int worldX, int worldZ) {
        glm::ivec3 chunkPos = {
            divFloor(worldX, Chunk::CHUNK_SIZE.x),
            0,
            divFloor(worldZ, Chunk::CHUNK_SIZE.z)
        };
        glm::ivec3 localPos = {
            worldX - chunkPos.x * Chunk::CHUNK_SIZE.x,
            Chunk::CHUNK_SIZE.y - 1, // start from top
            worldZ - chunkPos.z * Chunk::CHUNK_SIZE.z
        };
        int elevation = static_cast<int>(perlin.octave2D_01(worldX * 0.01f, worldZ * 0.01f, 6) * 80.0f);
        return elevation;
    }

    int getActualHeightAt(int worldX, int worldZ) {
        glm::ivec3 chunkPos = {
            divFloor(worldX, Chunk::CHUNK_SIZE.x),
            0,
            divFloor(worldZ, Chunk::CHUNK_SIZE.z)
        };
        Chunk* chunk = getChunkAt(chunkPos);
        if (!chunk) return -1; // chunk non généré

        glm::ivec3 localPos = {
            worldX - chunkPos.x * Chunk::CHUNK_SIZE.x,
            Chunk::CHUNK_SIZE.y - 1, // start from top
            worldZ - chunkPos.z * Chunk::CHUNK_SIZE.z
        };

        for (int y = Chunk::CHUNK_SIZE.y - 1; y >= 0; y--) {
            localPos.y = y;
            Block block = chunk->getBlockAt(localPos);
            if (block.type != AIR) {
                return y + chunkPos.y * Chunk::CHUNK_SIZE.y;
            }
        }
        return -1; // no solid block found
    }
    
    bool isBlockSolid(const glm::ivec3& worldPos) {
        // Calcul de la position du chunk correspondant
        glm::ivec3 chunkPos = {
            divFloor(worldPos.x, Chunk::CHUNK_SIZE.x),
            divFloor(worldPos.y, Chunk::CHUNK_SIZE.y),
            divFloor(worldPos.z, Chunk::CHUNK_SIZE.z)
        };

        Chunk* chunk = getChunkAt(chunkPos);
        if (!chunk) return false; // chunk non généré => bloc vide

        // Coordonnées locales dans le chunk
        glm::ivec3 localPos = worldPos - chunkPos * Chunk::CHUNK_SIZE;

        // Vérifie que la position est valide
        if (localPos.x < 0 || localPos.x >= Chunk::CHUNK_SIZE.x ||
            localPos.y < 0 || localPos.y >= Chunk::CHUNK_SIZE.y ||
            localPos.z < 0 || localPos.z >= Chunk::CHUNK_SIZE.z) {
            return false;
        }

        Block block = chunk->getBlockAt(localPos);
        return block.type != AIR;
    }


    void generateChunks(int radius, glm::ivec3 centerChunk) {
        for (int x = -radius; x <= radius; x++) {
            for (int z = -radius; z <= radius; z++) {
                glm::ivec3 chunkPos = centerChunk + glm::ivec3(x, 0, z);
                createChunkAt(chunkPos);
                generateStructureInChunk(chunkPos);
            }
        }
        std::cout << "Generated " << chunkMap.size() << " chunks.\n";
    }

    Chunk* getChunkAt(const glm::ivec3& pos) {
        auto it = chunkMap.find(pos);
        if (it != chunkMap.end())
            return it->second.get();  // shared_ptr -> raw pointer
        return nullptr;
    }

    
    void placeBlock(Block block, bool byUser = true) {
        glm::ivec3 chunkPos = glm::floor(glm::vec3(block.position) / glm::vec3(Chunk::CHUNK_SIZE));

        Chunk* chunk = getChunkAt(chunkPos);
        if (!chunk) {
            createChunkAt(chunkPos);        // créer le chunk si manquant
            chunk = getChunkAt(chunkPos);
            if (!chunk) return;             // sécurité absolue
        }

        glm::ivec3 localPos = block.position - chunkPos * Chunk::CHUNK_SIZE;

        if (localPos.x < 0 || localPos.x >= Chunk::CHUNK_SIZE.x ||
            localPos.y < 0 || localPos.y >= Chunk::CHUNK_SIZE.y ||
            localPos.z < 0 || localPos.z >= Chunk::CHUNK_SIZE.z) {
            std::cerr << "placeBlock: localPos hors limites ! " << localPos.x << ", " << localPos.y << ", " << localPos.z << std::endl;
            return;
        }

        chunk->setBlockAt(localPos, block.type);
        chunk->meshGenerated = false; // for regeneration
    }

    Block getBlockAt(const glm::ivec3& worldPos) {
        glm::ivec3 chunkPos = {
            divFloor(worldPos.x, Chunk::CHUNK_SIZE.x),
            divFloor(worldPos.y, Chunk::CHUNK_SIZE.y),
            divFloor(worldPos.z, Chunk::CHUNK_SIZE.z)
        };

        Chunk* chunk = getChunkAt(chunkPos);
        if (!chunk) {
            static Block airBlock = {{0,0,0}, AIR};
            return airBlock; // Return an AIR block if chunk doesn't exist
        }

        glm::ivec3 localPos = worldPos - chunkPos * Chunk::CHUNK_SIZE;

        if (localPos.x < 0 || localPos.x >= Chunk::CHUNK_SIZE.x ||
            localPos.y < 0 || localPos.y >= Chunk::CHUNK_SIZE.y ||
            localPos.z < 0 || localPos.z >= Chunk::CHUNK_SIZE.z) {
            static Block airBlock = {{0,0,0}, AIR};
            return airBlock; // Return an AIR block if out of bounds
        }

        return chunk->getBlockAt(localPos);
    }


    std::vector<glm::ivec3> getAllChunksToDraw(glm::ivec3 playerChunkPos, int viewDistance) {
        std::vector<glm::ivec3> chunksToDraw;
        for (int x = -viewDistance; x <= viewDistance; x++) {
            for (int z = -viewDistance; z <= viewDistance; z++) {
                glm::ivec3 chunkPos = playerChunkPos + glm::ivec3(x, 0, z);
                chunkPos.y = 0; // For now, only ground level
                // Check for distance to avoid square shape
                if (glm::length(glm::vec2(x, z)) > viewDistance) continue;
                chunksToDraw.push_back(chunkPos);
            }
        }
        return chunksToDraw;
    }

    void unloadFarChunks(glm::ivec3 playerChunkPos, int viewDistance) {
        std::vector<glm::ivec3> chunksToRemove;
        for (const auto& pair : chunkMap) {
            const glm::ivec3& chunkPos = pair.first;
            if (glm::length(glm::vec2(chunkPos.x - playerChunkPos.x, chunkPos.z - playerChunkPos.z)) > viewDistance + 2) {
                chunksToRemove.push_back(chunkPos);
            }
        }
        for (const auto& pos : chunksToRemove) {
            auto it = chunkMap.find(pos);
            if (it != chunkMap.end()) {
                // Save chunk to file before removing
                if (it->second->busy) {
                    std::cout << "Chunk at " << glm::to_string(pos) << " is busy, skipping unload.\n";
                    continue; // skip if busy
                }
                std::string filename = getFilenameForChunk(pos);
                auto chunk = it->second;    // shared_ptr garde vivant
                chunkMap.erase(it);          // supprime map, chunk reste alive si thread l’utilise
                chunk->saveToFile(filename);

                std::cout << "Unloaded chunk at " << glm::to_string(pos) << std::endl;
            }
        }
    }

    void removeBlock(const glm::ivec3& worldPos) {
        glm::ivec3 chunkPos = {
            divFloor(worldPos.x, Chunk::CHUNK_SIZE.x),
            divFloor(worldPos.y, Chunk::CHUNK_SIZE.y),
            divFloor(worldPos.z, Chunk::CHUNK_SIZE.z)
        };

        Chunk* chunk = getChunkAt(chunkPos);
        if (!chunk) return; // chunk non généré

        glm::ivec3 localPos = worldPos - chunkPos * Chunk::CHUNK_SIZE;

        if (localPos.x < 0 || localPos.x >= Chunk::CHUNK_SIZE.x ||
            localPos.y < 0 || localPos.y >= Chunk::CHUNK_SIZE.y ||
            localPos.z < 0 || localPos.z >= Chunk::CHUNK_SIZE.z) {
            return; // out of bounds
        }

        chunk->setBlockAt(localPos, AIR);
        chunk->meshGenerated = false; // for regeneration
    }

private:

int divFloor(int x, int size) {
    return x >= 0 ? x / size : (x - size + 1) / size;
}
};
