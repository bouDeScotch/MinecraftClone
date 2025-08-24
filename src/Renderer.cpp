#include "../include/Renderer.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#include <map>

static std::map<BlockType, BlockTexture> blockTextures = {
    {DIRT, {0, 0, 0}},
    {GRASS, {2, 1, 0}},
    {STONE, {3, 3, 3}},
    {WOOD, {7, 6, 7}},
    {LEAF, {5, 5, 5}},
    {SAND, {4, 4, 4}}
};

Renderer::Renderer() : cubeVAO(0), cubeVBO(0) {}

Renderer::~Renderer() {
    if(cubeVBO) glDeleteBuffers(1, &cubeVBO);
    if(cubeVAO) glDeleteVertexArrays(1, &cubeVAO);
}

void Renderer::init() {
    setupCube();
    loadTextures("../assets/textures/textureAtlas01.png");
}

void Renderer::loadTextures(std::string filepath) {
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        return;
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Paramètres de wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Paramètres de filtrage
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Choisir le bon format
    GLenum format = GL_RGB;
    if (nrChannels == 1) format = GL_RED;
    else if (nrChannels == 3) format = GL_RGB;
    else if (nrChannels == 4) format = GL_RGBA;

    // Upload des pixels vers la VRAM
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
                 0, format, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}


void Renderer::setupCube() {
    enum DIRS {
        UP = 0,
        SIDES = 1,
        DOWN = 2
    };

    
    Vertex vertices[] = {
        // Front face (z = +0.5)
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, SIDES, {0.0f, 0.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, SIDES, {0.0f, 0.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, SIDES, {0.0f, 0.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, SIDES, {0.0f, 0.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}, SIDES, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, SIDES, {0.0f, 0.0f, 1.0f}},
        // Back face (z = -0.5)
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, SIDES, {0.0f, 0.0f, -1.0f}},
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, SIDES, {0.0f, 0.0f, -1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, SIDES, {0.0f, 0.0f, -1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, SIDES, {0.0f, 0.0f, -1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, SIDES, {0.0f, 0.0f, -1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, SIDES, {0.0f, 0.0f, -1.0f}},
        // Left face (x = -0.5)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, SIDES, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, SIDES, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, SIDES, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, SIDES, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, SIDES, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, SIDES, {-1.0f, 0.0f, 0.0f}},
        // Right face (x = +0.5)
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, SIDES, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, SIDES, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, SIDES, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, SIDES, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, SIDES, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, SIDES, {1.0f, 0.0f, 0.0f}},
        // Top face (y = +0.5)
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}, UP, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, UP, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f}, UP, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f}, UP, {0.0f, 1.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f}, UP, {0.0f, 1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}, UP, {0.0f, 1.0f, 0.0f}},
        // Bottom face (y = -0.5)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, DOWN, {0.0f, -1.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, DOWN, {0.0f, -1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, DOWN, {0.0f, -1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, DOWN, {0.0f, -1.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, DOWN, {0.0f, -1.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, DOWN, {0.0f, -1.0f, 0.0f}},
    };
    
    // VBOs wich doesn't change between cubes
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLsizei stride = sizeof(Vertex);

    // Position
    // glVertexAttribPointer(GLunint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void * pointer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(1);

    // FaceID
    glVertexAttribIPointer(2, 1, GL_INT, stride, (void*)offsetof(Vertex, faceID));
    glEnableVertexAttribArray(2);

    // Normal
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    // Instance VBOs
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, Chunk::CHUNK_SIZE*Chunk::CHUNK_SIZE*Chunk::CHUNK_SIZE * sizeof(InstanceData), nullptr, GL_STATIC_DRAW);
    
    // Instance offset
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, offset));
    glVertexAttribDivisor(4, 1);

    // Instance texture
    // Top
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 1, GL_INT, sizeof(InstanceData), (void*)offsetof(InstanceData, topTexID));
    glVertexAttribDivisor(5, 1);
    // Side
    glEnableVertexAttribArray(6);
    glVertexAttribIPointer(6, 1, GL_INT, sizeof(InstanceData), (void*)offsetof(InstanceData, sideTexID));
    glVertexAttribDivisor(6, 1);
    // Bottom
    glEnableVertexAttribArray(7);
    glVertexAttribIPointer(7, 1, GL_INT, sizeof(InstanceData), (void*)offsetof(InstanceData, bottomTexID));
    glVertexAttribDivisor(7, 1);
}

void Renderer::drawChunk(const Chunk& chunk, Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    shader.use();

    // 1. Préparer les données instanciées
    std::vector<InstanceData> instances(chunk.blocks.size());
    for (size_t i = 0; i < chunk.blocks.size(); ++i) {
        instances[i].offset = chunk.blocks[i].position;
        BlockType type = chunk.blocks[i].type;
        if (blockTextures.find(type) != blockTextures.end()) {
            instances[i].topTexID = blockTextures[type].top;
            instances[i].sideTexID = blockTextures[type].side;
            instances[i].bottomTexID = blockTextures[type].bottom;
        } else {
            instances[i].topTexID = 0;
            instances[i].sideTexID = 0;
            instances[i].bottomTexID = 0;
        }
    }

    // 2. Mettre à jour le VBO instancié
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, instances.size() * sizeof(InstanceData), instances.data());

    // 3. Mettre en place les matrices
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    shader.setVec3("lightDir", glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)));

    // 3.5 Definir la texture
    shader.setInt("atlasSize", 4);

    // 4. Draw call
    glBindVertexArray(cubeVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, chunk.blocks.size());
    glBindVertexArray(0);
}
