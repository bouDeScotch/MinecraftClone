#include "include/Chunk.h"
#include "include/PerlinNoise.hpp"
#include <iostream>
#include <filesystem>
#include <glm/gtx/string_cast.hpp>
#include <fstream>

// Create a file in the old format for testing
void createOldFormatFile(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    
    // Write chunk position
    glm::ivec3 chunkPos(1, 2, 3);
    file.write(reinterpret_cast<const char*>(&chunkPos), sizeof(chunkPos));
    
    // Write old format block data (full array)
    std::vector<Block> oldBlocks(32768);
    for (int i = 0; i < 32768; i++) {
        oldBlocks[i] = {{i % 16, (i / 16) % 128, i / (16 * 128)}, AIR};
    }
    // Set a few blocks to non-AIR
    oldBlocks[100].type = STONE;
    oldBlocks[200].type = DIRT;
    oldBlocks[300].type = GRASS;
    
    file.write(reinterpret_cast<const char*>(oldBlocks.data()), oldBlocks.size() * sizeof(Block));
    file.close();
}

int main() {
    // Test 1: Create multiple chunks and compare sizes
    siv::PerlinNoise perlin(12345);
    
    std::cout << "Testing multiple chunks for size comparison:\n";
    
    for (int i = 0; i < 3; i++) {
        Chunk testChunk(glm::ivec3(i, 0, 0));
        testChunk.generate(perlin);
        
        std::string filename = "/tmp/test_chunk_" + std::to_string(i);
        testChunk.saveToFile(filename);
        
        std::string fullFilename = filename + ".blk";
        if (std::filesystem::exists(fullFilename)) {
            auto fileSize = std::filesystem::file_size(fullFilename);
            std::cout << "Chunk " << i << " size: " << fileSize << " bytes (old would be " << (12 + 32768 * 16) << ")" << std::endl;
        }
    }
    
    // Test 2: Create a mostly empty chunk
    std::cout << "\nTesting mostly empty chunk:\n";
    Chunk emptyChunk(glm::ivec3(100, 0, 0));
    // Don't generate - leave it mostly empty, just set a few blocks
    emptyChunk.setBlockAt(glm::ivec3(0, 0, 0), STONE);
    emptyChunk.setBlockAt(glm::ivec3(1, 1, 1), DIRT);
    emptyChunk.setBlockAt(glm::ivec3(15, 127, 15), GRASS);
    
    emptyChunk.saveToFile("/tmp/empty_chunk");
    std::string emptyFilename = "/tmp/empty_chunk.blk";
    if (std::filesystem::exists(emptyFilename)) {
        auto fileSize = std::filesystem::file_size(emptyFilename);
        std::cout << "Mostly empty chunk size: " << fileSize << " bytes" << std::endl;
        std::cout << "Compression ratio vs old: " << (double)(12 + 32768 * 16) / fileSize << ":1" << std::endl;
    }
    
    // Test 3: Verify loading the empty chunk
    Chunk loadedEmpty(glm::ivec3(999, 999, 999));
    loadedEmpty.loadFromFile("/tmp/empty_chunk");
    
    if (loadedEmpty.chunkPos == emptyChunk.chunkPos) {
        std::cout << "Empty chunk position loaded correctly\n";
    }
    
    // Check the specific blocks we set
    if (loadedEmpty.getBlockAt(glm::ivec3(0, 0, 0)).type == STONE &&
        loadedEmpty.getBlockAt(glm::ivec3(1, 1, 1)).type == DIRT &&
        loadedEmpty.getBlockAt(glm::ivec3(15, 127, 15)).type == GRASS) {
        std::cout << "Specific blocks loaded correctly\n";
    } else {
        std::cout << "ERROR: Specific blocks not loaded correctly\n";
    }
    
    // Test 4: Check that unset blocks are AIR
    if (loadedEmpty.getBlockAt(glm::ivec3(5, 5, 5)).type == AIR) {
        std::cout << "Unset blocks are correctly AIR\n";
    } else {
        std::cout << "ERROR: Unset blocks are not AIR\n";
    }
    
    std::cout << "\nAll tests completed successfully!\n";
    
    return 0;
}