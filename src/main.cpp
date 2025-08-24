#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "../include/Renderer.h"
#include "../include/World.h"
#include "../include/Player.h"
#include "../include/Camera.h"
#include "../include/Shader.h"

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

Player player;
Camera camera;
bool firstMouse = true;

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


int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mini Voxel Game", NULL, NULL);
    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    // Désactive VSync
    glfwSwapInterval(0);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Renderer renderer;
    renderer.init();
    World world;
    Shader shader("../shaders/vertex.glsl", "../shaders/fragment.glsl");

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
        }

        processInput(window, deltaTime); // tu pourras utiliser deltaTime pour le mouvement

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(player.position, player.position + camera.front, camera.up);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);

        renderer.drawChunk(world.chunk, shader, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

