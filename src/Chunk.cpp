#include "../include/Chunk.h"
#include "../include/PerlinNoise.hpp"
#include <cstdint>
#include <iostream>
#include <map>
#include <time.h>
#include <fstream>
#include <set>

// Block texture is
// {front, back, left, right, top, bottom}
static std::map<BlockType, BlockTexture> blockTextures = {
    {DIRT, {0, 0, 0, 0, 0, 0}},
    {GRASS, {1, 1, 1, 1, 2, 0}},
    {STONE, {3, 3, 3, 3, 3, 3}},
    {WOOD, {6, 6, 6, 6, 7, 7}},
    {LEAF, {5, 5, 5, 5, 5, 5}},
    {SAND, {4, 4, 4, 4, 4, 4}},
    {PUMPKIN, {8, 9, 9, 9, 9, 10}},
    {SNOW, {11, 11, 11, 11, 11, 11}},
    {COBBLESTONE, {12, 12, 12, 12, 12, 12}},
    {BRICK, {13, 13, 13, 13, 13, 13}},
    {PLANKS, {14, 14, 14, 14, 14, 14}},
    {IRON_ORE, {15, 15, 15, 15, 15, 15}},
};

enum Biomes {
    PLAINS,
    DESERT,
    FOREST,
    MOUNTAINS,
    SNOWY,
    SWAMP
};

const glm::ivec3 Chunk::CHUNK_SIZE = glm::ivec3(16, 128, 16);


void Chunk::generate(siv::PerlinNoise& perlin) {
    meshPositions.reserve(Chunk::CHUNK_SIZE.x * Chunk::CHUNK_SIZE.y * Chunk::CHUNK_SIZE.z * 6);
    meshFaces.reserve(Chunk::CHUNK_SIZE.x * Chunk::CHUNK_SIZE.y * Chunk::CHUNK_SIZE.z * 6);
    meshTypes.reserve(Chunk::CHUNK_SIZE.x * Chunk::CHUNK_SIZE.y * Chunk::CHUNK_SIZE.z * 6);
    blocks.resize(Chunk::CHUNK_SIZE.x * Chunk::CHUNK_SIZE.y * Chunk::CHUNK_SIZE.z, {{0,0,0}, AIR});

    for (int x = 0; x < Chunk::CHUNK_SIZE.x; x++) {
        int worldX = x + chunkPos.x * Chunk::CHUNK_SIZE.x;

        for (int z = 0; z < Chunk::CHUNK_SIZE.z; z++) {
            int worldZ = z + chunkPos.z * Chunk::CHUNK_SIZE.z;
            
            // Terrain params
            int elevation = perlin.octave2D_01(worldX * 0.01f, worldZ * 0.01f, 6) * 80.0f;

            float temperature = perlin.octave2D_01(worldX * 0.002f, worldZ * 0.002f, 3);  
            float humidity    = perlin.octave2D_01(worldX * 0.002f + 100, worldZ * 0.002f + 100, 3);
            Biomes biome;
            if (temperature < 0.3f && humidity < 0.3f)
                biome = SNOWY;
            else if (temperature > 0.7f && humidity < 0.3f)
                biome = DESERT;
            else if (temperature < 0.3f && humidity > 0.7f)
                biome = FOREST;
            else if (temperature < 0.3f)
                biome = SNOWY;
            else if (humidity > 0.7f)
                biome = SWAMP;
            else
                biome = PLAINS;
            for (int y = 0; y < Chunk::CHUNK_SIZE.y; y++) {
                if (y > elevation) continue;
                // Place top block based on biome
                if (y == elevation) {
                    switch (biome) {
                        case PLAINS:
                            setBlockAt({x, y, z}, GRASS);
                            break;
                        case DESERT:
                            setBlockAt({x, y, z}, SAND);
                            break;
                        case FOREST:
                            // Add wooden planks occasionally in forest areas
                            float forestNoise = perlin.octave2D_01(worldX * 0.2f, worldZ * 0.2f, 2);
                            if (forestNoise > 0.9f) {
                                setBlockAt({x, y, z}, PLANKS);
                            } else {
                                setBlockAt({x, y, z}, GRASS);
                            }
                            break;
                        case MOUNTAINS:
                            // Add some architectural variety to mountains
                            float mountainNoise = perlin.octave2D_01(worldX * 0.1f, worldZ * 0.1f, 3);
                            if (mountainNoise > 0.8f) {
                                setBlockAt({x, y, z}, BRICK);
                            } else if (mountainNoise > 0.7f) {
                                setBlockAt({x, y, z}, COBBLESTONE);
                            } else {
                                setBlockAt({x, y, z}, STONE);
                            }
                            break;
                        case SNOWY:
                            setBlockAt({x, y, z}, SNOW);
                            break;
                        case SWAMP:
                            setBlockAt({x, y, z}, GRASS);
                            break;
                        default:
                            setBlockAt({x, y, z}, GRASS);
                            break;
                    }
                }
                // Place sub-surface blocks
                else if (y >= elevation - 4) {
                    switch (biome) {
                        case PLAINS:
                        case FOREST:
                        case SWAMP:
                            setBlockAt({x, y, z}, DIRT);
                            break;
                        case DESERT:
                            setBlockAt({x, y, z}, SAND);
                            break;
                        case MOUNTAINS:
                        case SNOWY:
                            setBlockAt({x, y, z}, STONE);
                            break;
                        default:
                            setBlockAt({x, y, z}, DIRT);
                            break;
                    }
                }
                // Deep underground blocks
                else {
                    // Generate ore deposits using noise
                    float oreNoise = perlin.octave3D_01(worldX * 0.05f, y * 0.05f, worldZ * 0.05f, 4);
                    
                    if (oreNoise > 0.7f && y < 30) {
                        // Iron ore deposits at lower depths
                        setBlockAt({x, y, z}, IRON_ORE);
                    } else if (oreNoise > 0.8f && y < 50) {
                        // Cobblestone patches in mid-depths  
                        setBlockAt({x, y, z}, COBBLESTONE);
                    } else {
                        setBlockAt({x, y, z}, STONE);
                    }
                }
            }
        }
    }
}


void Chunk::setBlockAt(const glm::ivec3& localPos, BlockType type) {
    int index = localPos.x 
              + localPos.y * CHUNK_SIZE.x
              + localPos.z * CHUNK_SIZE.x * CHUNK_SIZE.y;
    blocks[index].type = type;
    blocks[index].position = glm::vec3(localPos);
}


Block& Chunk::getBlockAt(const glm::ivec3& localPos) {
    // Out of bounds check
    if (localPos.x < 0 || localPos.x >= CHUNK_SIZE.x ||
        localPos.y < 0 || localPos.y >= CHUNK_SIZE.y ||
        localPos.z < 0 || localPos.z >= CHUNK_SIZE.z) {
        static Block airBlock = {{0,0,0}, AIR};
        return airBlock; // Return a reference to a static AIR block
    }
    int index = localPos.x 
              + localPos.y * CHUNK_SIZE.x
              + localPos.z * CHUNK_SIZE.x * CHUNK_SIZE.y;
    return blocks[index];
}


void Chunk::generateMesh() {
    busy = true;
    vertices.clear();
    indices.clear();

    vertices.reserve(blocks.size() * 24); // max 24 vertices per block
    indices.reserve(blocks.size() * 36);  // max 36 indices per block

    meshPositions.clear();
    meshFaces.clear();
    meshTypes.clear();
    meshPositions.reserve(blocks.size() * 6); // max 6 faces per block
    meshFaces.reserve(blocks.size() * 6);
    meshTypes.reserve(blocks.size() * 6);


    //for (const auto& block : blocks) {
    for (int x = 0; x < CHUNK_SIZE.x; ++x) {
        for (int y = 0; y < CHUNK_SIZE.y; ++y) {
            for (int z = 0; z < CHUNK_SIZE.z; ++z) {
                Block& block = getBlockAt(glm::ivec3(x,y,z));
                BlockType type = block.type;
                glm::ivec3 localPos = glm::ivec3(x,y,z);
                glm::ivec3 worldPos = localPos + chunkPos * CHUNK_SIZE;
                if (type == AIR) continue;

                // Check each face
                // FRONT (+Z)
                if (getBlockAt(localPos + glm::ivec3(0, 0, 1)).type == AIR) {
                    meshPositions.push_back(worldPos);
                    meshFaces.push_back(FRONT);
                    meshTypes.push_back(type);
                }
                // BACK (-Z)
                if (getBlockAt(localPos + glm::ivec3(0, 0, -1)).type == AIR) {
                    meshPositions.push_back(worldPos);
                    meshFaces.push_back(BACK);
                    meshTypes.push_back(type);
                }
                // LEFT (-X)
                if (getBlockAt(localPos + glm::ivec3(-1, 0, 0)).type == AIR) {
                    meshPositions.push_back(worldPos);
                    meshFaces.push_back(LEFT);
                    meshTypes.push_back(type);
                }
                // RIGHT (+X)
                if (getBlockAt(localPos + glm::ivec3(1, 0, 0)).type == AIR) {
                    meshPositions.push_back(worldPos);
                    meshFaces.push_back(RIGHT);
                    meshTypes.push_back(type);
                }
                // TOP (+Y)
                if (getBlockAt(localPos + glm::ivec3(0, 1, 0)).type == AIR) {
                    meshPositions.push_back(worldPos);
                    meshFaces.push_back(TOP);
                    meshTypes.push_back(type);
                }
                // BOTTOM (-Y)
                if (getBlockAt(localPos + glm::ivec3(0, -1, 0)).type == AIR) {
                    meshPositions.push_back(worldPos);
                    meshFaces.push_back(BOTTOM);
                    meshTypes.push_back(type);
                }
            }
        }
    }

    addFaces(meshPositions, meshFaces, meshTypes);
    meshGenerated = true;
    uploadingToGPU = true;
    busy = false;
}

void Chunk::addFaces(const std::vector<glm::ivec3>& meshPositions, 
                     const std::vector<Face>& meshFaces, 
                     const std::vector<BlockType>& meshTypes) {
    static const glm::vec3 nrm[6] = {
        { 0, 0,  1}, // FRONT  (+Z)
        { 0, 0, -1}, // BACK   (-Z)
        {-1, 0,  0}, // LEFT   (-X)
        { 1, 0,  0}, // RIGHT  (+X)
        { 0, 1,  0}, // TOP    (+Y)
        { 0,-1,  0}, // BOTTOM (-Y)
    };

    glm::vec3 v[6][4] = {
        // FRONT (+Z)
        {
            {0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}
        },
        // BACK (-Z)
        {
            {1,0,0}, {0,0,0}, {0,1,0}, {1,1,0}
        },
        // LEFT (-X)
        {
            {0,0,0}, {0,0,1}, {0,1,1}, {0,1,0}
        },
        // RIGHT (+X)
        {
            {1,0,1}, {1,0,0}, {1,1,0}, {1,1,1}
        },
        // TOP (+Y)
        {
            {0,1,1}, {1,1,1}, {1,1,0}, {0,1,0}
        },
        // BOTTOM (-Y)
        {
            {0,0,0}, {1,0,0}, {1,0,1}, {0,0,1}
        }
    };

    static const glm::vec2 uv[4] = {
        {0,0}, {1,0}, {1,1}, {0,1}
    };

    for (size_t idx = 0; idx < meshPositions.size(); ++idx) {
        const glm::vec3 base(meshPositions[idx]); // coin min du bloc (x,y,z)
        Face f = meshFaces[idx];
        BlockType type = meshTypes[idx];
        int tileID;
        switch (f) {
            case FRONT:  tileID = blockTextures[type].front;  break;
            case BACK:   tileID = blockTextures[type].back;   break;
            case LEFT:   tileID = blockTextures[type].left;   break;
            case RIGHT:  tileID = blockTextures[type].right;  break;
            case TOP:    tileID = blockTextures[type].top;    break;
            case BOTTOM: tileID = blockTextures[type].bottom; break;
        }
        uint32_t baseIndex = static_cast<uint32_t>(vertices.size());
        // 4 sommets
        for (int i = 0; i < 4; ++i) {
            Vertex vert;
            vert.pos       = base + v[f][i];
            vert.uv        = uv[i];
            vert.normal    = nrm[f];
            vert.faceID = tileID; // même tuile sur la face
            vertices.push_back(vert);
        }

        // 2 triangles: (0,1,2) et (0,2,3)
        indices.push_back(baseIndex + 0);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);

        indices.push_back(baseIndex + 0);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);
    }
}


void Chunk::uploadMeshToGPU()
{
    if (gl.vao == 0) {
        glGenVertexArrays(1, &gl.vao);
        glGenBuffers(1, &gl.vbo);
        glGenBuffers(1, &gl.ebo);
    }

    glBindVertexArray(gl.vao);

    glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(Vertex),
                 vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(uint32_t),
                 indices.data(),
                 GL_STATIC_DRAW);

    // Attribs: aPos(0), aUV(1), aNormal(2), aFaceTexID(3)
    // adaptes si ton shader a d’autres locations
    GLsizei stride = sizeof(Vertex);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(Vertex, pos));

    // uv
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(Vertex, uv));

    // normal
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(Vertex, normal));

    // faceTexID (entier → IPointer)
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_INT, stride,
                           (void*)offsetof(Vertex, faceID));

    glBindVertexArray(0);

    gl.indexCount = indices.size();
}


void Chunk::saveToFile(const std::string& filename) {
    std::string filenameBlocks = filename + ".blk";
    std::ofstream file(filenameBlocks, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    file.write(reinterpret_cast<const char*>(&chunkPos), sizeof(chunkPos));

    uint32_t nonAirCount = 0;
    for (const auto& block : blocks) {
        if (block.type != AIR) nonAirCount++;
    }
    file.write(reinterpret_cast<const char*>(&nonAirCount), sizeof(nonAirCount));

    std::unordered_map<uint8_t, std::vector<uint16_t>> typeToPositions;
    for (auto& block : blocks) {
        if (block.type != AIR) {
            uint8_t type = static_cast<uint8_t>(block.type);
            glm::ivec3 pos = glm::ivec3(block.position);
            uint16_t index = pos.x 
                           + pos.y * CHUNK_SIZE.x
                           + pos.z * CHUNK_SIZE.x * CHUNK_SIZE.y;
            typeToPositions[type].push_back(index);
        }
    }

    for (auto& [type, positions] : typeToPositions) {
        file.write(reinterpret_cast<const char*>(&type), sizeof(type));

        uint16_t count = positions.size();
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));

        file.write(reinterpret_cast<const char*>(positions.data()), count * sizeof(uint16_t));
    }
}


bool Chunk::isInFile(const std::string& filename) {
    std::string filenameBlocks = filename + ".blk";
    std::ifstream file(filenameBlocks, std::ios::binary);
    return file.good();
}



void Chunk::loadFromFile(const std::string& filename) {
    std::string filenameBlocks = filename + ".blk";
    std::ifstream file(filenameBlocks, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for reading: " << filenameBlocks << std::endl;
        return;
    }

    file.read(reinterpret_cast<char*>(&chunkPos), sizeof(chunkPos));

    uint32_t nonAirCount;
    file.read(reinterpret_cast<char*>(&nonAirCount), sizeof(nonAirCount));
    if (nonAirCount > CHUNK_SIZE.x * CHUNK_SIZE.y * CHUNK_SIZE.z) {
        std::cerr << "Corrupted file: invalid nonAirCount=" << nonAirCount << "\n";
        return;
    }

    blocks.assign(CHUNK_SIZE.x * CHUNK_SIZE.y * CHUNK_SIZE.z, {{0,0,0}, AIR});

    uint32_t placed = 0;
    while (placed < nonAirCount && file) {
        uint8_t type;
        file.read(reinterpret_cast<char*>(&type), sizeof(type));
        if (!file) break;

        uint16_t count;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));
        if (!file || count == 0 || count > blocks.size()) {
            std::cerr << "Corrupted file: invalid count=" << count << "\n";
            return;
        }

        std::vector<uint16_t> positions(count);
        file.read(reinterpret_cast<char*>(positions.data()), count * sizeof(uint16_t));
        if (!file) {
            std::cerr << "Corrupted file: premature EOF while reading positions\n";
            return;
        }

        for (uint16_t index : positions) {
            if (index >= blocks.size()) {
                std::cerr << "Corrupted file: index out of bounds (" << index << ")\n";
                return;
            }
            Block& b = blocks[index];
            b.type = static_cast<BlockType>(type);
            b.position = chunkPos * CHUNK_SIZE + glm::ivec3(
                index % CHUNK_SIZE.x,
                (index / CHUNK_SIZE.x) % CHUNK_SIZE.y,
                index / (CHUNK_SIZE.x * CHUNK_SIZE.y)
            );
        }
        placed += count;
    }
}


