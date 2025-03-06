#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "ShapeManager.h"
#include "Shader.h"

// -----------------------------------------------
// FUNCTION DEFINITIONS
// -----------------------------------------------
void generateCircleVertices(std::vector<float>& circleVertices, glm::vec3 pos, float radius, int segments);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void collision(glm::vec3& position, glm::vec2& velocity, float radius);

// -----------------------------------------------
// GLOBAL VARIABLES
// -----------------------------------------------
#define SCR_WIDTH 800
#define SCR_HEIGHT 800

using namespace std;

int main() {
    // -----------------------------------------------
    // SETUP GLFW
    // -----------------------------------------------
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Create GLFW Window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Gravity Simulation", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    // Set context as current window
    glfwMakeContextCurrent(window);
    // Set callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // -----------------------------------------------
    // LOAD GLAD
    // -----------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // -----------------------------------------------
    // SETUP SHADER
    // -----------------------------------------------
    Shader myShader("vertexShader.vert", "fragmentShader.frag");

    // -----------------------------------------------
    // CREATE CIRCLE
    // -----------------------------------------------	
    float radius = 0.15f;
    int segments = 25;
    const float gravitationalConst = -9.81f;
    glm::vec2 velocity(0.85f, 0.0f);
    std::vector<float> circleVertices;
    glm::vec3 position(0.0f, 0.0f, 0.0f);
    // Generate the circle circleVertices
    generateCircleVertices(circleVertices, position, radius, segments);
    // Create the circle shape
    ShapeManager circle;
    int circleIndex = circle.createShape(circleVertices.data(), circleVertices.size() * sizeof(float));
    circle.addAttribute(circleIndex, 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    // -----------------------------------------------
    // CREATE DIRECTION LINE
    // -----------------------------------------------
    float lineVertices[] = { 0.0f, 0.0f, radius, 0.0f };
    ShapeManager line;
	int lineIndex = line.createShape(lineVertices, sizeof(lineVertices));
	line.addAttribute(lineIndex, 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    // Variables for tracking time
    float lastFrame = 0.0f;
    float deltaTime = 0.0f;

    // -----------------------------------------------
    // MAIN LOOP
    // -----------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        // Calculate deltaTime
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process Input
        processInput(window);

        // Specify the color of the background
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // Clean the back buffer and assign the new color to it
        glClear(GL_COLOR_BUFFER_BIT);


        // -----------------------------------------------
        // MOVE THE BALL
        // -----------------------------------------------
        // Set uniform
        myShader.use();
        myShader.setVec2("position", position);
        // Move downwards with acceleration 
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
        velocity.y += gravitationalConst * deltaTime;
        
        
        // -----------------------------------------------
        // BORDER COLLISION 
        // -----------------------------------------------
        if (velocity.y != 0.0f || velocity.x != 0.0f) {
            collision(position, velocity, radius);
        }
        cout << "velocity.x: " << velocity.x << endl;
        cout << "velocity.y: " << velocity.y << endl;
        

        // -----------------------------------------------
        // RENDER
        // -----------------------------------------------
        // Render shape
        myShader.setVec3("color", glm::vec3(1.0f, 1.0f, 0.0f));
        circle.renderShape(circleIndex, sizeof(float) * 3, GL_TRIANGLE_FAN);
        // Render Line
		myShader.setVec3("color", glm::vec3(0.0f, 0.0f, 1.0f));
		glLineWidth(2.0f);
		line.renderShape(lineIndex, 2, GL_LINES);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // -----------------------------------------------
    // CLEANUP
    // -----------------------------------------------
    circle.~ShapeManager();
    line.~ShapeManager();
    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void generateCircleVertices(std::vector<float>& circleVertices, glm::vec3 pos, float radius, int segments) {
    // Center position of the circle
    circleVertices.push_back(pos.x);
    circleVertices.push_back(pos.y);
    circleVertices.push_back(pos.z);

    // Generate circle circleVertices
    for (int i = 0; i <= segments; i++) {
        float angle = (2.0f * glm::pi<float>() * i) / segments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        circleVertices.push_back(x);
        circleVertices.push_back(y);
        circleVertices.push_back(pos.z);
    }
}

void collision(glm::vec3& position, glm::vec2& velocity, float radius) {
    const float EPSILON = 0.05f; // Small threshold for stopping
    const float FRICTION = -0.75f; // Friction factor (adjust as needed)

    // Bottom
    if (position.y <= (-1.0f + radius)) {
        position.y = -1.0f + radius;
        velocity.y *= FRICTION;
        if (std::abs(velocity.y) < EPSILON) 
            velocity.y = 0.0f;
    }
    // Top
    if (position.y >= (1.0f - radius)) {
        position.y = 1.0f - radius;
        velocity.y *= FRICTION;
        if (std::abs(velocity.y) < EPSILON) 
            velocity.y = 0.0f;
    }
    // Right
    if (position.x >= (1.0f - radius)) {
        position.x = 1.0f - radius;
        velocity.x *= FRICTION;
        if (std::abs(velocity.x) < EPSILON) 
            velocity.x = 0.0f;
    }
    // Left
    if (position.x <= (-1.0f + radius)) {
        position.x = -1.0f + radius;
        velocity.x *= FRICTION;
        if (std::abs(velocity.x) < EPSILON) 
            velocity.x = 0.0f;
    }
}

// -----------------------------------------------
// TASKS
// -----------------------------------------------
// TODO Implement collision between balls
// FIX Update the ball position without artefacts