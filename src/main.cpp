#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <future>

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

int main() {
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

    Renderer renderer;
    renderer.init();
    World world;
    world.generateChunks(3, glm::ivec3(0,0,0));
    std::cout << "Starting mesh generation for " << world.chunks.size() << " chunks...\n";
    /*
    for (auto& chunk : world.chunks) {
        chunk.generateMesh();
    }
    */

    std::vector<std::future<void>> futures;
    /*
    for (auto& chunk : world.chunks) {
        futures.push_back(std::async(std::launch::async, [&chunk]() {
            chunk.generateMesh();
        }));
    }

    for (auto& fut : futures) {
        fut.get();
    }
    */

    size_t n = world.chunks.size();
    futures.reserve(n);
    unsigned threads = std::max(1u, std::min((unsigned)n, std::thread::hardware_concurrency()));
    // simple job system : spawn worker threads that pop indices from an atomic counter
    std::atomic<size_t> idx{0};
    std::vector<std::thread> workers;
    for (unsigned t=0; t<threads; ++t) {
        workers.emplace_back([&](){
            while (true) {
                size_t i = idx.fetch_add(1);
                if (i >= n) break;
                world.chunks[i].generateMesh(); // safe because no reallocation occurs
            }
        });
    }
    for (auto& th : workers) th.join();

    for (auto& chunk : world.chunks) {
        chunk.uploadMeshToGPU();
    }
    std::cout << "Mesh generation completed.\n";
    Shader shader("../shaders/vertex.glsl", "../shaders/fragment.glsl");

    /*
    Player Position: (118.059, 137.031, -89.8551)
    Camera Direction: (498.202, -41.5001)
    */
    player.position = glm::vec3(118.0f, 137.0f, 114.0f);
    camera.position = player.position;
    camera.yaw = 226.0f;
    camera.pitch = -34.0f;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    int frames = 0;
    float fpsTimer = 0.0f;

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

            // Also print player position and camera direction
            std::cout << "Player Position: ("
                        << player.position.x << ", "
                        << player.position.y << ", "
                        << player.position.z << ")\n";
            std::cout << "Camera Direction: ("
                        << camera.yaw << ", "
                    << camera.pitch << ")" << std::endl;
        }

        processInput(window, deltaTime); // tu pourras utiliser deltaTime pour le mouvement

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(player.position, player.position + camera.front, camera.up);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 1000.0f);

        for (const auto& chunk : world.chunks) {
            renderer.drawChunkMesh(chunk, shader, view, projection, -camera.front);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

