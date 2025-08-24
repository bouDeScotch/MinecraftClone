#include "../include/Chunk.h"

Chunk::Chunk() {
    generate();
}

void Chunk::generate() {
    // Génère un simple cube 1x1x1 à l'origine
    blocks.push_back({glm::vec3(0,0,0)});
}
