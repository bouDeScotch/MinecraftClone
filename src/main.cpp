#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <future>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>


#include "../include/Renderer.h"
#include "../include/World.h"
#include "../include/Player.h"
#include "../include/Camera.h"
#include "../include/Shader.h"

unsigned int windowedWidth = 1280, windowedHeight = 720;
unsigned int SCR_WIDTH = windowedWidth;
unsigned int SCR_HEIGHT = windowedHeight;
bool cursorDisabled = true;
bool isFullscreen = false;

Player player;
Camera camera;
World world;
Renderer renderer;

void processInput(GLFWwindow *window, float deltaTime) {
    float speed = 10.0f * deltaTime;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        player.move(camera.front * speed);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        player.move(-camera.front * speed);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        player.move(glm::normalize(glm::cross(camera.front, camera.up)) * -speed);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        player.move(glm::normalize(glm::cross(camera.front, camera.up)) * speed);
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        player.move(camera.up * speed);
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        player.move(-camera.up * speed);
    // Detect right click and place a block
    static bool wasPressed = false;
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (!wasPressed) {
            player.placeBlock(world, BlockType::LEAF, camera);
            wasPressed = true;
        }
    } else {
        wasPressed = false;
    }

    static bool escWasPressed = false;
    int escState = glfwGetKey(window, GLFW_KEY_ESCAPE);
    if (escState == GLFW_PRESS && !escWasPressed) {
        cursorDisabled = !cursorDisabled;
        glfwSetInputMode(window, GLFW_CURSOR, cursorDisabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        escWasPressed = true;

        if (cursorDisabled) {
            // Recenter the cursor
            glfwSetCursorPos(window, SCR_WIDTH / 2.0, SCR_HEIGHT / 2.0);
        }
    } else if (escState == GLFW_RELEASE) {
        escWasPressed = false;
    }

    static bool f11WasPressed = false;
    int f11State = glfwGetKey(window, GLFW_KEY_F11);
    if (f11State == GLFW_PRESS && !f11WasPressed) {
        isFullscreen = !isFullscreen;
        if (isFullscreen) {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            SCR_WIDTH = mode->width;
            SCR_HEIGHT = mode->height;
        } else {
            glfwSetWindowMonitor(window, NULL, 100, 100, windowedWidth, windowedHeight, 0);
            SCR_WIDTH = windowedWidth;
            SCR_HEIGHT = windowedHeight;
        }
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        f11WasPressed = true;
    } else if (f11State == GLFW_RELEASE) {
        f11WasPressed = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = SCR_WIDTH / 2.0f;
    static float lastY = SCR_HEIGHT / 2.0f;
    static bool firstMouse = true;

    if(firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // inversion
    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0,0,width,height);
}

glm::vec3 getLightDir() {
    // Use time to create a moving light direction
    float time = glfwGetTime() * 0.1f; // Slow down the movement
    static float theta = .3; // Angle d'inclinaison avec le plan (Oxy)
    float z = sin(theta);
    float x = cos(time) * cos(theta);
    float y = sin(time) * cos(theta);
    return glm::normalize(glm::vec3(x, y, z));
}

int main() {
    World world;
    
    std::queue<Chunk*> chunksToUpload;
    std::mutex chunksMutex;
    std::atomic<bool> generatorRunning{true};

    std::thread chunkGenerator([&world, &chunksToUpload, &chunksMutex, &generatorRunning](){
        while(generatorRunning) {
            for(auto& [pos, chunk] : world.chunkMap) {
                if(!chunk.meshGenerated) {
                    chunk.generateMesh();
                    {
                        std::lock_guard<std::mutex> lock(chunksMutex);
                        chunksToUpload.push(&chunk);
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(4));
        }
    });

    glfwInit();
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mini Voxel Game", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    // Désactive VSync
    glfwSwapInterval(0);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    renderer.init();
    std::cout << "Generating chunks...\n";
    world.generateChunks(8, glm::ivec3(0,0,0));
    Shader shader("../shaders/vertex.glsl", "../shaders/fragment.glsl");
    Shader sunShader("../shaders/sun.vert", "../shaders/sun.frag");

    player.position = glm::vec3(0.0f, 137.0f, 0.0f);
    camera.position = player.position;
    camera.yaw = 226.0f;
    camera.pitch = -34.0f;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    int frames = 0;
    float fpsTimer = 0.0f;

    std::cout << "Entering main loop...\n";
    while(!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        fpsTimer += deltaTime;
        frames++;
        if(fpsTimer >= 1.0f) {
            std::cout << "FPS: " << frames << std::endl;
            frames = 0;
            fpsTimer = 0.0f;
        }

        processInput(window, deltaTime);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(player.position, player.position + camera.front, camera.up);
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 10000.0f);

        glm::ivec3 playerChunkPos = {
            static_cast<int>(std::floor(player.position.x / Chunk::CHUNK_SIZE.x)),
            static_cast<int>(std::floor(player.position.y / Chunk::CHUNK_SIZE.y)),
            static_cast<int>(std::floor(player.position.z / Chunk::CHUNK_SIZE.z))
        };

        auto chunksToDraw = world.getAllChunksToDraw(playerChunkPos, 20);

        // Créer les chunks manquants (toujours main thread)
        for(const auto& pos : chunksToDraw) {
            if(!world.getChunkAt(pos)) {
                world.createChunkAt(pos);
            }
        }

        // Upload sur GPU les chunks prêts
        {
            std::lock_guard<std::mutex> lock(chunksMutex);
            while(!chunksToUpload.empty()) {
                Chunk* c = chunksToUpload.front();
                chunksToUpload.pop();
                c->uploadMeshToGPU();
            }
        }

        // Draw chunks
        for(const auto& pos : chunksToDraw) {
            Chunk* chunk = world.getChunkAt(pos);
            if(chunk && chunk->meshGenerated) {
                renderer.drawChunkMesh(*chunk, shader, view, projection, getLightDir());
            }
        }

        renderer.drawSun(sunShader, view, projection, getLightDir());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    generatorRunning = false;
    if(chunkGenerator.joinable())
        chunkGenerator.join();
    glfwTerminate();
    return 0;
}

