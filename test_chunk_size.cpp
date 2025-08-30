#include "include/Chunk.h"
#include "include/PerlinNoise.hpp"
#include <iostream>
#include <filesystem>
#include <glm/gtx/string_cast.hpp>

int main() {
    // Test sparse chunk storage
    siv::PerlinNoise perlin(12345);
    
    // Create a test chunk
    Chunk testChunk(glm::ivec3(0, 0, 0));
    testChunk.generate(perlin);
    
    // Save the chunk
    std::string filename = "/tmp/test_chunk";
    testChunk.saveToFile(filename);
    
    // Check file size
    std::string fullFilename = filename + ".blk";
    if (std::filesystem::exists(fullFilename)) {
        auto fileSize = std::filesystem::file_size(fullFilename);
        std::cout << "Generated chunk file size: " << fileSize << " bytes" << std::endl;
        std::cout << "For comparison, old format would be: " << (12 + 32768 * 16) << " bytes" << std::endl;
        std::cout << "Compression ratio: " << (double)(12 + 32768 * 16) / fileSize << ":1" << std::endl;
    }
    
    // Test loading the chunk
    Chunk loadedChunk(glm::ivec3(1, 1, 1));
    loadedChunk.loadFromFile(filename);
    
    // Verify the chunk position was loaded correctly
    if (loadedChunk.chunkPos == testChunk.chunkPos) {
        std::cout << "Chunk position loaded correctly: " << glm::to_string(loadedChunk.chunkPos) << std::endl;
    } else {
        std::cout << "ERROR: Chunk position mismatch!" << std::endl;
    }
    
    // Check a few blocks to verify content
    int nonAirBlocks = 0;
    int matchingBlocks = 0;
    for (int x = 0; x < Chunk::CHUNK_SIZE.x; x++) {
        for (int y = 0; y < Chunk::CHUNK_SIZE.y; y++) {
            for (int z = 0; z < Chunk::CHUNK_SIZE.z; z++) {
                glm::ivec3 pos(x, y, z);
                Block& originalBlock = testChunk.getBlockAt(pos);
                Block& loadedBlock = loadedChunk.getBlockAt(pos);
                
                if (originalBlock.type != AIR) {
                    nonAirBlocks++;
                }
                
                if (originalBlock.type == loadedBlock.type) {
                    matchingBlocks++;
                }
            }
        }
    }
    
    std::cout << "Non-AIR blocks in original: " << nonAirBlocks << std::endl;
    std::cout << "Matching blocks after load: " << matchingBlocks << " / " << (32768) << std::endl;
    
    if (matchingBlocks == 32768) {
        std::cout << "SUCCESS: All blocks match!" << std::endl;
    } else {
        std::cout << "ERROR: Block mismatch detected!" << std::endl;
    }
    
    return 0;
}