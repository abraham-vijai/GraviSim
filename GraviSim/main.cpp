#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

// -----------------------------------------------
// GLOBAL VARIABLES
// -----------------------------------------------
#define SCR_WIDTH 800
#define SCR_HEIGHT 800
float lastFrameTime = 0.0f;
bool isPressed = false;
glm::vec2 startPos(0.0f, 0.0f);
glm::vec2 endPos(0.0f, 0.0f);
Ball ball(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), 0.15f, 25);

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

	//srand(static_cast<unsigned int>(time(0)));
	//// -----------------------------------------------
	//// CREATE RANDOM VALUES 
	//// -----------------------------------------------
	//float randRadius = static_cast<float>(rand() % 100) / 100.0f;
	//float randX = static_cast<float>(rand() % 200) / 100.0f - 1.0f;
	//float randY = static_cast<float>(rand() % 200) / 100.0f - 1.0f;
	//float randVelX = static_cast<float>(rand() % 200) / 100.0f - 1.0f;
	//float randVelY = static_cast<float>(rand() % 200) / 100.0f - 1.0f;
	//float randColorR = static_cast<float>(rand() % 100) / 100.0f;
	//float randColorG = static_cast<float>(rand() % 100) / 100.0f;
	//float randColorB = static_cast<float>(rand() % 100) / 100.0f;

	// -----------------------------------------------
	// SETUP SHADER
	// -----------------------------------------------
	Shader myShader("vertexShader.vert", "fragmentShader.frag");
	Shader pullLineShader("vertexShaderLine.vert", "fragmentShader.frag");

	// -----------------------------------------------
	// CREATE CIRCLE
	// -----------------------------------------------	
	std::vector<float> circleVertices;
	// Generate the circle circleVertices
	ball.generateBallVertices(circleVertices);
	// Create the circle shape
	ShapeManager circle;
	int circleIndex = circle.createShape(circleVertices.data(), circleVertices.size() * sizeof(float));
	circle.addAttribute(circleIndex, 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// -----------------------------------------------
	// CREATE DIRECTION LINE
	// -----------------------------------------------
	float directionLineVertices[] = { 0.0f, 0.0f, ball.radius, 0.0f };
	ShapeManager directionLine;
	int directionLineIndex = directionLine.createShape(directionLineVertices, sizeof(directionLineVertices));
	directionLine.addAttribute(directionLineIndex, 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	// -----------------------------------------------
	// CREATE PULL LINE
	// -----------------------------------------------
	float pullLineVertices[] = { 0.0f,0.0f,0.0f,0.0f };
	ShapeManager pullLine;
	int pullLineIndex = pullLine.createShape(pullLineVertices, sizeof(pullLineVertices), GL_DYNAMIC_DRAW);
	pullLine.addAttribute(pullLineIndex, 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	// -----------------------------------------------
	// MAIN LOOP
	// -----------------------------------------------
	while (!glfwWindowShouldClose(window)) {
		// Process keyboard Input
		processKeyBoard(window);

		float currentTime = static_cast<float>(glfwGetTime());
		float deltaTime = currentTime - lastFrameTime;
		lastFrameTime = currentTime;

		// -----------------------------------------------
		// UPDATE PULL LINE
		// -----------------------------------------------
		// Update pull line vertices
		pullLineVertices[0] = ball.position.x;
		pullLineVertices[1] = ball.position.y;
		pullLineVertices[2] = endPos.x;
		pullLineVertices[3] = endPos.y;
		// Update the pull line buffer
		pullLine.updateBuffer(pullLineIndex, pullLineVertices, sizeof(pullLineVertices));

		// Specify the color of the background
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);

		// -----------------------------------------------
		// MOVE THE BALL
		// -----------------------------------------------
		// Set uniform
		myShader.use();
		myShader.setVec3("position", ball.position);
		ball.updatePhysics(deltaTime);

		// -----------------------------------------------
		// RENDER
		// -----------------------------------------------
		// Render shape
		myShader.setVec3("color", glm::vec3(1.0f, 1.0f, 0.0f));
		circle.renderShape(circleIndex, sizeof(float) * 3, GL_TRIANGLE_FAN);
		// Render Line
		myShader.setVec3("color", glm::vec3(0.0f, 0.0f, 1.0f));
		glLineWidth(2.0f);
		directionLine.renderShape(directionLineIndex, 2, GL_LINES);

		// Process Mouse Input
		processMouse(window, pullLineShader, pullLine, pullLineIndex);

		// Swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// -----------------------------------------------
	// CLEANUP
	// -----------------------------------------------
	circle.~ShapeManager();
	directionLine.~ShapeManager();
	pullLine.~ShapeManager();
	glfwTerminate();

	return 0;
}

void processKeyBoard(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void processMouse(GLFWwindow* window, Shader& pullLineShader, ShapeManager& pullLine, int pullLineindex) {
	if (isPressed) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		// Convert to OpenGL coordinates
		endPos.x = (static_cast<float>(xpos) / SCR_WIDTH) * 2.0f - 1.0f;
		endPos.y = 1.0f - (static_cast<float>(ypos) / SCR_HEIGHT) * 2.0f;

		// Render pull Line
		pullLineShader.use();
		pullLineShader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
		glLineWidth(2.0f);
		pullLine.renderShape(pullLineindex, 8, GL_LINES);
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

			// Calculate the direction of the pull line
			float pullLineLength = glm::distance(startPos, endPos);
			glm::vec2 vectorComponents = endPos - startPos;
			float magnitude = glm::length(vectorComponents);
			glm::vec2 pullLineDirection = glm::vec2(vectorComponents.x / magnitude, vectorComponents.y / magnitude);
			pullLineDirection *= pullLineLength;

			// Update the ball velocity
			ball.velocity = -pullLineDirection;
		}
	}
}

// -----------------------------------------------
// TASKS
// -----------------------------------------------
// TODO Implement collision between balls
// FIX Reduce global variables
// TODO Add multiple balls with different colors
// TODO The pull line should be associated with only one ball