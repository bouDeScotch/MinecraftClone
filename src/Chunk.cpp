#include "../include/Chunk.h"
#include "../include/PerlinNoise.hpp"
#include <iostream>
#include <map>
#include <time.h>

// Block texture is
// {front, back, left, right, top, bottom}
static std::map<BlockType, BlockTexture> blockTextures = {
    {DIRT, {0, 0, 0, 0, 0, 0}},
    {GRASS, {1, 1, 1, 1, 2, 0}},
    {STONE, {3, 3, 3, 3, 3, 3}},
    {WOOD, {6, 6, 6, 6, 7, 7}},
    {LEAF, {5, 5, 5, 5, 5, 5}},
    {SAND, {4, 4, 4, 4, 4, 4}}
};

static const siv::PerlinNoise::seed_type seed = time(0);

const glm::ivec3 Chunk::CHUNK_SIZE = glm::ivec3(16, 128, 16);

void Chunk::generate() {
    
    meshPositions.reserve(Chunk::CHUNK_SIZE.x * Chunk::CHUNK_SIZE.y * Chunk::CHUNK_SIZE.z * 6);
    meshFaces.reserve(Chunk::CHUNK_SIZE.x * Chunk::CHUNK_SIZE.y * Chunk::CHUNK_SIZE.z * 6);
    meshTypes.reserve(Chunk::CHUNK_SIZE.x * Chunk::CHUNK_SIZE.y * Chunk::CHUNK_SIZE.z * 6);
    blocks.resize(Chunk::CHUNK_SIZE.x * Chunk::CHUNK_SIZE.y * Chunk::CHUNK_SIZE.z, {{0,0,0}, AIR});


    siv::PerlinNoise perlin(seed);

    for (float x = 0; x < Chunk::CHUNK_SIZE.x; x++) {
        int worldX = x + chunkPos.x * Chunk::CHUNK_SIZE.x;
        for (float z = 0; z < Chunk::CHUNK_SIZE.z; z++) {
            int worldZ = z + chunkPos.z * Chunk::CHUNK_SIZE.z;
            // Simple height map using sine waves
            float height = perlin.octave2D_01(worldX*0.05f, worldZ*0.05f, 4) * 50.0f;
            float humidity = perlin.octave2D_01(worldX*0.01f + 100, worldZ*0.01f + 100, 4);
            for (float y = 0; y < height && y < Chunk::CHUNK_SIZE.y; y++) {
                int worldY = y + chunkPos.y * Chunk::CHUNK_SIZE.y;
                // Place random BlockType for demonstration
                if (worldY < height - 3) {
                    setBlockAt(glm::ivec3(x, y, z), STONE);
                } else if (worldY < height - 1) {
                    if (humidity < 0.3f) {
                        setBlockAt(glm::ivec3(x, y, z), SAND);
                    } else {
                        setBlockAt(glm::ivec3(x, y, z), DIRT);
                    }
                } else {
                    if (humidity < 0.3f) {
                        setBlockAt(glm::ivec3(x, y, z), SAND);
                    } else {
                        setBlockAt(glm::ivec3(x, y, z), GRASS);
                    }
                }
            }
        }
    }
}

const void Chunk::setBlockAt(const glm::ivec3& localPos, BlockType type) {
    int index = localPos.x 
              + localPos.y * CHUNK_SIZE.x
              + localPos.z * CHUNK_SIZE.x * CHUNK_SIZE.y;
    blocks[index].type = type;
    blocks[index].position = glm::vec3(localPos);
}


Block& Chunk::getBlockAt(const glm::ivec3& localPos) {
    int index = localPos.x 
              + localPos.y * CHUNK_SIZE.x
              + localPos.z * CHUNK_SIZE.x * CHUNK_SIZE.y;
    return blocks[index];
}


void Chunk::generateMesh() {
    vertices.clear();
    indices.clear();

    vertices.reserve(blocks.size() * 24); // max 24 vertices per block
    indices.reserve(blocks.size() * 36);  // max 36 indices per block

    meshPositions.clear();
    meshFaces.clear();
    meshTypes.clear();

    for (const auto& block : blocks) {
        glm::ivec3 localPos = glm::ivec3(block.position);
        glm::ivec3 worldPos = localPos + chunkPos * CHUNK_SIZE;
        BlockType type = block.type;
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

    addFaces(meshPositions, meshFaces, meshTypes);
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
