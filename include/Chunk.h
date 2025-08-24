#pragma once
#include <glm/glm.hpp>
#include <vector>

enum BlockType {
    AIR,
    DIRT,
    GRASS,
    STONE,
    WOOD,
    LEAF
};

struct Block {
    glm::vec3 position;
    BlockType type;
};

class Chunk {
public:
    std::vector<Block> blocks;
    Chunk();

    void generate();
};
