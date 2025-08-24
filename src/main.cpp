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

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Player player;
Camera camera;
float lastX = SCR_WIDTH/2, lastY = SCR_HEIGHT/2;
bool firstMouse = true;

void processInput(GLFWwindow *window) {
    float speed = 0.05f;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        player.move(camera.front * speed);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        player.move(-camera.front * speed);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        player.move(glm::normalize(glm::cross(camera.front, camera.up)) * -speed);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        player.move(glm::normalize(glm::cross(camera.front, camera.up)) * speed);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if(firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos; lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // rotation simple du front
    glm::vec3 front;
    front.x = cos(glm::radians(yoffset)) * cos(glm::radians(xoffset));
    front.y = sin(glm::radians(yoffset));
    front.z = cos(glm::radians(yoffset)) * sin(glm::radians(xoffset));
    camera.front = glm::normalize(front);
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mini Voxel Game", NULL, NULL);
    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Renderer renderer;
    renderer.init();
    World world;
    Shader shader("../shaders/vertex.glsl", "../shaders/fragment.glsl");

    while(!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.1f,0.1f,0.15f,1.0f);
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
