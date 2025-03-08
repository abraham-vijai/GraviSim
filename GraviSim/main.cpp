#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <random>

#include "ShapeManager.h"
#include "Shader.h"
#include "Ball.h"

// -----------------------------------------------
// FUNCTION DEFINITIONS
// -----------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processMouse(GLFWwindow* window, Shader& pullLineShader, ShapeManager& pullLine, int pullLineindex);
void processKeyBoard(GLFWwindow* window);
float getRandomFloat(float min, float max);
void convertToOpenGLCoordinates(double xpos, double ypos, float& mouseX, float& mouseY);

// -----------------------------------------------
// GLOBAL VARIABLES
// -----------------------------------------------
#define SCR_WIDTH 800
#define SCR_HEIGHT 800
const float distThreshold = 0.1f;
float lastFrameTime = 0.0f;
bool isPressed = false;
glm::vec2 startPos(0.0f, 0.0f);
glm::vec2 endPos(0.0f, 0.0f);
std::vector<Ball> ballList;
Ball* selectedBall = nullptr;

using namespace std;

#include <iostream>

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
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// -----------------------------------------------
	// LOAD GLAD
	// -----------------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	// -----------------------------------------------
	// CREATE MULTIPLE BALLS
	// -----------------------------------------------
	for (size_t i = 0; i < 5; i++)
	{
		// -----------------------------------------------
		// CREATE RANDOM VALUES 
		// -----------------------------------------------
		float randRadius = getRandomFloat(0.1f, 0.3f);
		float randX = getRandomFloat(-1.0f + randRadius, 1.0f - randRadius);
		float randY = getRandomFloat(-1.0f + randRadius, 1.0f - randRadius);
		float randVelX = 0.0f;
		float randVelY = 0.0f;
		float randColorR = getRandomFloat(0.0f, 1.0f);
		float randColorG = getRandomFloat(0.0f, 1.0f);
		float randColorB = getRandomFloat(0.0f, 1.0f);

		// Initalize the ball
		Ball ball(glm::vec3(randX, randY, 0.0f), glm::vec2(randVelX, randVelY), glm::vec3(randColorR, randColorG, randColorB), randRadius, 25);

		// Add the balls to the vector
		ballList.push_back(ball);
	}

	// -----------------------------------------------
	// SETUP SHADER
	// -----------------------------------------------
	Shader myShader("vertexShader.vert", "fragmentShader.frag");
	Shader pullLineShader("vertexShaderLine.vert", "fragmentShader.frag");

	// -----------------------------------------------
	// CREATE PULL LINE
	// -----------------------------------------------
	float pullLineVertices[] = { 0.0f,0.0f,0.0f,0.0f };
	ShapeManager pullLine;
	int pullLineIndex = pullLine.createShape(pullLineVertices, sizeof(pullLineVertices), GL_DYNAMIC_DRAW);
	pullLine.addAttribute(pullLineIndex, 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	// -----------------------------------------------
	// CREATE DIRECTION LINE
	// -----------------------------------------------
	float directionLineVertices[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ShapeManager directionLine;
	int directionLineIndex = directionLine.createShape(directionLineVertices, sizeof(directionLineVertices));
	directionLine.addAttribute(directionLineIndex, 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);


	std::vector<float> circleVertices;
	ShapeManager circle;


	// -----------------------------------------------
	// MAIN LOOP
	// -----------------------------------------------
	while (!glfwWindowShouldClose(window)) {
		// Process keyboard Input
		processKeyBoard(window);

		float currentTime = static_cast<float>(glfwGetTime());
		float deltaTime = currentTime - lastFrameTime;
		lastFrameTime = currentTime;

		// Specify the color of the background
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);


		for (Ball& newBall : ballList)
		{
			// -----------------------------------------------
			// UPDATE PULL LINE
			// -----------------------------------------------
			if (selectedBall != nullptr) {
				// Update pull line vertices
				pullLineVertices[0] = selectedBall->position.x;
				pullLineVertices[1] = selectedBall->position.y;
				pullLineVertices[2] = endPos.x;
				pullLineVertices[3] = endPos.y;
				// Update the pull line buffer
				pullLine.updateBuffer(pullLineIndex, pullLineVertices, sizeof(pullLineVertices));
			}
			// -----------------------------------------------
			// UPDATE DIRECTION LINE
			// -----------------------------------------------
			directionLineVertices[0] = 0.0f;
			directionLineVertices[1] = 0.0f;
			directionLineVertices[2] = newBall.radius;
			directionLineVertices[3] = 0.0f;
			directionLine.updateBuffer(directionLineIndex, directionLineVertices, sizeof(directionLineVertices));

			// -----------------------------------------------
			// CREATE CIRCLE
			// -----------------------------------------------				
			// Generate the circle circleVertices
			newBall.generateBallVertices(circleVertices);
			int circleIndex = circle.createShape(circleVertices.data(), circleVertices.size() * sizeof(float));
			circle.addAttribute(circleIndex, 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			circleVertices.clear();

			// -----------------------------------------------
			// MOVE THE BALL
			// -----------------------------------------------
			// Set uniform
			myShader.use();
			myShader.setVec3("position", newBall.position);
			newBall.updatePhysics(deltaTime);

			// -----------------------------------------------
			// RENDER
			// -----------------------------------------------
			// Render shape
			myShader.setVec3("color", newBall.color);
			circle.renderShape(circleIndex, sizeof(float) * 3, GL_TRIANGLE_FAN);
			// Render Line
			myShader.setVec3("color", glm::vec3(0.0f, 0.0f, 0.0f));
			glLineWidth(2.0f);
			directionLine.renderShape(directionLineIndex, 2, GL_LINES);
		}

		// Process Mouse Input
		processMouse(window, pullLineShader, pullLine, pullLineIndex);

		// Swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

void processKeyBoard(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void processMouse(GLFWwindow* window, Shader& pullLineShader, ShapeManager& pullLine, int pullLineindex) {
    if (!isPressed || selectedBall == nullptr) return;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    convertToOpenGLCoordinates(xpos, ypos, endPos.x, endPos.y);

    // Render pull line
    pullLineShader.use();
    pullLineShader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
    glLineWidth(2.0f);
    pullLine.renderShape(pullLineindex, 8, GL_LINES);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void convertToOpenGLCoordinates(double xpos, double ypos, float& mouseX, float& mouseY) {
	mouseX = (static_cast<float>(xpos) / SCR_WIDTH) * 2.0f - 1.0f;
	mouseY = 1.0f - (static_cast<float>(ypos) / SCR_HEIGHT) * 2.0f;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	auto isPointInCircle = [](float x, float y, const Ball& ball) {
		float dx = x - ball.position.x;
		float dy = y - ball.position.y;
		return (dx * dx + dy * dy) < (ball.radius * ball.radius);
		};

	double xpos, ypos;
	float mouseX, mouseY;

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		glfwGetCursorPos(window, &xpos, &ypos);
		convertToOpenGLCoordinates(xpos, ypos, mouseX, mouseY);

		if (action == GLFW_PRESS) {
			isPressed = true;
			selectedBall = nullptr;

			// Find the selected ball
			for (auto& newBall : ballList) {
				if (isPointInCircle(mouseX, mouseY, newBall)) {
					selectedBall = &newBall;
					break;
				}
			}
		}
		else if (action == GLFW_RELEASE) {
			isPressed = false;

			if (selectedBall) {
				glfwGetCursorPos(window, &xpos, &ypos);
				convertToOpenGLCoordinates(xpos, ypos, mouseX, mouseY);

				endPos = glm::vec2(mouseX, mouseY);
				startPos = selectedBall->position;
				glm::vec2 vectorComponents = endPos - startPos;
				float magnitude = glm::length(vectorComponents);

				if (magnitude > 0.0001f) {  // Prevent division by zero
					glm::vec2 pullLineDirection = vectorComponents / magnitude * glm::distance(startPos, endPos);
					selectedBall->velocity = -pullLineDirection;
				}

				// Reset selection after release
				selectedBall = nullptr;  
			}
		}
	}
}

float getRandomFloat(float min, float max) {
	// Create a random device to seed the random number generator
	std::random_device rd;

	// Initialize a Mersenne Twister random number generator with the seed from random_device
	std::mt19937 gen(rd());

	// Define a uniform real distribution for the range [min, max]
	std::uniform_real_distribution<float> dis(min, max);

	// Generate and return a random float
	return dis(gen);
}

// -----------------------------------------------
// TASKS
// -----------------------------------------------
// TODO Implement collision between balls
// FIX Reduce global variables
// FIX Ball is now a global variable