#pragma once
#include "Chunk.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

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
    void drawChunkMesh(const Chunk& chunk, Shader& shader,
                       const glm::mat4& view, const glm::mat4& projection,
                       const glm::vec3 &lightDir = glm::vec3(0.5f));

    void drawSun(Shader& shader, const glm::mat4& view, const glm::mat4& projection, glm::vec3 lightDir);
private:
    unsigned int cubeVAO, cubeVBO;

    GLuint instanceVBO;

    GLuint sunVAO, sunVBO;

    void setupCube();
    void loadTextures(std::string filepath);
    glm::vec3 getLightDir(float time);
};

