#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Block {
    glm::vec3 position;
};

class Chunk {
public:
    std::vector<Block> blocks;
    Chunk();

    void generate();
};
