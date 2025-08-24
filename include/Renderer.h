#pragma once
#include "Chunk.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

class Renderer {
public:
    Renderer();
    ~Renderer();

    void init();
    void drawChunk(const Chunk& chunk, Shader& shader, const glm::mat4& view, const glm::mat4& projection);
private:
    unsigned int cubeVAO, cubeVBO;
    void setupCube();
};
