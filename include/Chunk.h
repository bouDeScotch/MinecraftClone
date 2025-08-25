#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>

enum BlockType {
    AIR,
    DIRT,
    GRASS,
    STONE,
    WOOD,
    LEAF,
    SAND,
    SNOW
};

enum Face {
    FRONT = 0,
    BACK = 1,
    LEFT = 2,
    RIGHT = 3,
    TOP = 4,
    BOTTOM = 5
};

struct ChunkMeshGL {
    GLuint vao = 0, vbo = 0, ebo = 0;
    size_t indexCount = 0;
};

struct BlockTexture {
    int front;
    int back;
    int left;
    int right;
    int top;
    int bottom;
};

struct Block {
    glm::vec3 position;
    BlockType type;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
    int faceID;
    glm::vec3 normal;
};

class Chunk {
public:
    std::vector<Block> blocks;

    static const glm::ivec3 CHUNK_SIZE;
    glm::ivec3 chunkPos;

    ChunkMeshGL gl;

   Chunk(const glm::ivec3& pos) : chunkPos(pos) { generate(); }

    void generate();

    void generateMesh();
    Block& getBlockAt(const glm::ivec3& localPos);
    const void setBlockAt(const glm::ivec3& localPos, BlockType type);
    void uploadMeshToGPU();

private:
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices;

    std::vector<glm::ivec3> meshPositions;
    std::vector<Face>      meshFaces;
    std::vector<BlockType> meshTypes;

    //void addFace(const glm::ivec3& bpos, Face f, int tileID);
    void addFaces(const std::vector<glm::ivec3>& positions, 
                  const std::vector<Face>& faces, 
                  const std::vector<BlockType>& types);
};
