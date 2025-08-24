#include "../include/Renderer.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer() : cubeVAO(0), cubeVBO(0) {}

Renderer::~Renderer() {
    if(cubeVBO) glDeleteBuffers(1, &cubeVBO);
    if(cubeVAO) glDeleteVertexArrays(1, &cubeVAO);
}

void Renderer::init() {
    setupCube();
}

void Renderer::setupCube() {
    enum DIRS {
        UP = 0,
        SIDES = 1,
        DOWN = 2
    };


    float vertices[] = {
        // positions          
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    int faceIDs[] = {
        SIDES, SIDES, SIDES, SIDES, SIDES, SIDES,
        SIDES, SIDES, SIDES, SIDES, SIDES, SIDES,
        SIDES, SIDES, SIDES, SIDES, SIDES, SIDES,
        SIDES, SIDES, SIDES, SIDES, SIDES, SIDES,
        DOWN,  DOWN,  DOWN,  DOWN,  DOWN,  DOWN,
        UP,    UP,    UP,    UP,    UP,    UP
    };

    
    // VBO pour positions
    GLuint VBO_pos, VBO_faceID;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO_pos);
    glGenBuffers(1, &VBO_faceID);

    glBindVertexArray(cubeVAO);

    // positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // face IDs (int)
    glBindBuffer(GL_ARRAY_BUFFER, VBO_faceID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(faceIDs), faceIDs, GL_STATIC_DRAW);
    glVertexAttribIPointer(1, 1, GL_INT, sizeof(int), (void*)0); // I pour int
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void Renderer::drawChunk(const Chunk& chunk, Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    glBindVertexArray(cubeVAO);
    shader.use();
    for(const auto& block : chunk.blocks) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), block.position);

        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);
}
