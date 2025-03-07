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
void generateCircleVertices(std::vector<float>& circleVertices, float radius, int segments);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processMouse(GLFWwindow* window, Shader& lineShader, ShapeManager& tempLine, int tempLineindex);
void processKeyBoard(GLFWwindow* window);
void updatePhysics();

// -----------------------------------------------
// GLOBAL VARIABLES
// -----------------------------------------------
#define SCR_WIDTH 800
#define SCR_HEIGHT 800
float radius = 0.15f;
float lastFrameTime = 0.0f;
const float damping = 0.8f; // Friction factor (adjust as needed)
const float velocityThreshold = 0.01f; // Define a small threshold
const float gravity = -9.81f;
bool isPressed = false;
glm::vec2 velocity(0.99f, 0.0f);
glm::vec3 position(0.5f, 0.5f, 0.0f); // Initial position of the circle
glm::vec2 startPos(0.0f, 0.0f);
glm::vec2 endPos(0.0f, 0.0f);

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
	// SETUP SHADER
	// -----------------------------------------------
	Shader myShader("vertexShader.vert", "fragmentShader.frag");
	Shader lineShader("vertexShaderLine.vert", "fragmentShader.frag");

	// -----------------------------------------------
	// CREATE CIRCLE
	// -----------------------------------------------	
	int segments = 25;
	std::vector<float> circleVertices;
	// Generate the circle circleVertices
	generateCircleVertices(circleVertices, radius, segments);
	// Create the circle shape
	ShapeManager circle;
	int circleIndex = circle.createShape(circleVertices.data(), circleVertices.size() * sizeof(float));
	circle.addAttribute(circleIndex, 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// -----------------------------------------------
	// CREATE DIRECTION LINE
	// -----------------------------------------------
	float directionLine[] = { 0.0f, 0.0f, radius, 0.0f };
	ShapeManager line;
	int lineIndex = line.createShape(directionLine, sizeof(directionLine));
	line.addAttribute(lineIndex, 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	// -----------------------------------------------
	// CREATE TEMPORARY LINE
	// -----------------------------------------------
	float tempLineVertices[] = { 0.0f,0.0f,0.0f,0.0f };
	ShapeManager tempLine;
	int tempLineIndex = tempLine.createShape(tempLineVertices, sizeof(tempLineVertices), GL_DYNAMIC_DRAW);
	tempLine.addAttribute(tempLineIndex, 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	// -----------------------------------------------
	// MAIN LOOP
	// -----------------------------------------------
	while (!glfwWindowShouldClose(window)) {
		// Process keyboard Input
		processKeyBoard(window);


		// -----------------------------------------------
		// UPDATE TEMPORARY LINE
		// -----------------------------------------------
		// Update temporary line vertices
		tempLineVertices[0] = position.x;
		tempLineVertices[1] = position.y;
		tempLineVertices[2] = endPos.x;
		tempLineVertices[3] = endPos.y;
		// Update the temporary line buffer
		tempLine.updateBuffer(tempLineIndex, tempLineVertices, sizeof(tempLineVertices));

		// Specify the color of the background
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);

		// -----------------------------------------------
		// MOVE THE BALL
		// -----------------------------------------------
		// Set uniform
		myShader.use();
		myShader.setVec3("position", position);
		updatePhysics();

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

		// Process Mouse Input
		processMouse(window, lineShader, tempLine, tempLineIndex);
		

		// Swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// -----------------------------------------------
	// CLEANUP
	// -----------------------------------------------
	circle.~ShapeManager();
	line.~ShapeManager();
	tempLine.~ShapeManager();
	glfwTerminate();

	return 0;
}

void updatePhysics() {
	float currentTime = static_cast<float>(glfwGetTime());
	float deltaTime = currentTime - lastFrameTime;
	lastFrameTime = currentTime;

	// Apply gravity when not hiting the ground
	if (position.y - radius > -1.0f) {
		velocity.y += gravity * deltaTime;
	}

	// Update position
	position.x += velocity.x * deltaTime;
	position.y += velocity.y * deltaTime;

	// Check for left/right wall collisions
	if (position.x + radius >= 1.0f || position.x - radius <= -1.0f) {
		velocity.x *= -damping;  // Reverse X velocity with damping
		position.x = (position.x + radius >= 1.0f) ? 1.0f - radius : -1.0f + radius;

		if (fabs(velocity.x) < velocityThreshold) velocity.x = 0.0f;
	}

	// Check for top/bottom collisions
	if (position.y - radius <= -1.0f) { // Bottom
		velocity.y *= -damping;
		position.y = -1.0f + radius;  // Prevent sinking into the ground

		// Apply friction when the ball is touching the ground
		velocity.x *= damping;

		// Stop bouncing and sliding if velocity is very low
		if (fabs(velocity.y) < velocityThreshold) velocity.y = 0.0f;
		if (fabs(velocity.x) < velocityThreshold) velocity.x = 0.0f;
	}
	else if (position.y + radius >= 1.0f) { // Top
		velocity.y *= -damping;  // Reverse Y velocity
		position.y = 1.0f - radius;

		if (fabs(velocity.y) < velocityThreshold) velocity.y = 0.0f;
	}
}

void processKeyBoard(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void processMouse(GLFWwindow* window, Shader& lineShader, ShapeManager& tempLine, int tempLineindex) {
	if (isPressed) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		// Convert to OpenGL coordinates
		endPos.x = (static_cast<float>(xpos) / SCR_WIDTH) * 2.0f - 1.0f;
		endPos.y = 1.0f - (static_cast<float>(ypos) / SCR_HEIGHT) * 2.0f;

		// Render Temporary Line
		lineShader.use();
		lineShader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f)); // Line colo
		glLineWidth(2.0f);
		tempLine.renderShape(tempLineindex, 8, GL_LINES);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			isPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			isPressed = false;
		}
	}
}

void generateCircleVertices(std::vector<float>& circleVertices, float radius, int segments) {
	// Center position of the circle
	circleVertices.push_back(0.0f);
	circleVertices.push_back(0.0f);
	circleVertices.push_back(0.0f);

	// Generate circle circleVertices
	for (int i = 0; i <= segments; i++) {
		float angle = (2.0f * glm::pi<float>() * i) / segments;
		float x = radius * cos(angle);
		float y = radius * sin(angle);
		circleVertices.push_back(x);
		circleVertices.push_back(y);
		circleVertices.push_back(0.0f);
	}
}

// -----------------------------------------------
// TASKS
// -----------------------------------------------
// TODO Implement collision between balls
// TODO Setup a line when left mouse click
// FIX Reduce global variables