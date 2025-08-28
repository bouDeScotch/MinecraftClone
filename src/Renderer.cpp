#include "../include/Renderer.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"


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



    // Not a texture but will go here for now
    // Triangle covering whole screen
    float crosshairVertices[] = {
        -1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f,
         1.0f, -1.0f
    };
    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);
    glBindVertexArray(crosshairVAO);

    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

struct SunVertex {
    glm::vec3 pos;
    glm::vec2 uv;
};

void Renderer::drawSun(Shader& shader, const glm::mat4& view, const glm::mat4& projection, glm::vec3 lightDir) {
    glm::vec3 lightDirNorm = glm::normalize(lightDir);
    glm::vec3 sunPos = lightDirNorm * 1000.0f;

    static const float size = 100.0f;

    glm::vec3 cameraRight = glm::vec3(view[0][0], view[1][0], view[2][0]);
    glm::vec3 cameraUp    = glm::vec3(view[0][1], view[1][1], view[2][1]);

    // Créer les sommets avec UV
    std::vector<SunVertex> quadVerts = {
        {sunPos - cameraRight*size - cameraUp*size, glm::vec2(0.0f, 0.0f)}, // bas gauche
        {sunPos + cameraRight*size - cameraUp*size, glm::vec2(1.0f, 0.0f)}, // bas droite
        {sunPos + cameraRight*size + cameraUp*size, glm::vec2(1.0f, 1.0f)}, // haut droite
        {sunPos - cameraRight*size + cameraUp*size, glm::vec2(0.0f, 1.0f)}  // haut gauche
    };

    if (sunVBO == 0) {
        glGenVertexArrays(1, &sunVAO);
        glGenBuffers(1, &sunVBO);
    }

    glBindVertexArray(sunVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sunVBO);
    glBufferData(GL_ARRAY_BUFFER, quadVerts.size() * sizeof(SunVertex), quadVerts.data(), GL_DYNAMIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SunVertex), (void*)0);

    // UV
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SunVertex), (void*)offsetof(SunVertex, uv));

    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setVec3("sunColor", glm::vec3(1.0f, 1.0f, 0.8f));

    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void Renderer::drawCrosshair(Shader& shader) { 
    shader.use();
    shader.setVec3("color", glm::vec3(1.0f));
    shader.setFloat("size", 10.0f);
    glBindVertexArray(crosshairVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, pos));
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
    glBufferData(GL_ARRAY_BUFFER, Chunk::CHUNK_SIZE.x*Chunk::CHUNK_SIZE.y*Chunk::CHUNK_SIZE.z * sizeof(InstanceData), nullptr, GL_STATIC_DRAW);
    
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

void Renderer::drawChunkMesh(const Chunk& chunk, Shader& shader,
                             const glm::mat4& view, const glm::mat4& projection,
                             const glm::vec3 &lightDir) {
    if (chunk.gl.indexCount == 0) return;

    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setMat4("model", glm::mat4(1.0f));
    shader.setInt("atlasSize", /* ex */ 4);
    shader.setVec3("lightDir", lightDir);

    glBindVertexArray(chunk.gl.vao);
    glDrawElements(GL_TRIANGLES,
                   static_cast<GLsizei>(chunk.gl.indexCount),
                   GL_UNSIGNED_INT,
                   0);
    glBindVertexArray(0);
}
