#pragma once
#include "Chunk.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

struct Vertex {
    glm::vec3 position;
    glm::vec2 uv;
    int faceID;
};

struct InstanceData {
    glm::vec3 offset;
    int topTexID;
    int sideTexID;
    int bottomTexID;
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    void init();
    void drawChunk(const Chunk& chunk, Shader& shader, const glm::mat4& view, const glm::mat4& projection);
private:
    unsigned int cubeVAO, cubeVBO;

    GLuint instanceVBO;

    void setupCube();
    void loadTextures(std::string filepath);
};

