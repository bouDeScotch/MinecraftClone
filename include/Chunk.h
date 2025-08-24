#pragma once
#include <glm/glm.hpp>
#include <vector>

enum BlockType {
    AIR,
    DIRT,
    GRASS,
    STONE,
    WOOD,
    LEAF,
    SAND
};

struct BlockTexture {
    int top;
    int side;
    int bottom;
};

struct Block {
    glm::vec3 position;
    BlockType type;
};

class Chunk {
public:
    std::vector<Block> blocks;

    static const int CHUNK_SIZE = 16;

    Chunk();

    void generate();
};
