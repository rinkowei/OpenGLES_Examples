
#define GLFW_INCLUDE_ES32
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Common/camera.h>
#include <Common/shader.h>
#include <Common/textureLoader.h>
#include <Common/model.h>

#include <iostream>
#include <array>
#include <map>
using namespace std;

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xPos, double yPos);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void handleInput(GLFWwindow* window);

const GLuint SCREEN_WIDTH = 800;
const GLuint SCREEN_HEIGHT = 600;

const string resources_dir(ES_EXAMPLE_RESOURCES_DIR);

Camera camera((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f, glm::vec3(0.0f, 10.0f, 30.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastTime = 0.0f;

int main()
{
	// initialize and configure glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_ES_API, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGLES_Examples", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	
	// capture mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD\n";
		return -1;
	}
	else
	{
		cout << "Success to initialize GLAD\n";
	}

	// enable depth test
	glEnable(GL_DEPTH_TEST);
	
	Shader constructionShader(resources_dir + "shaders/15.offscreen_rendering/construction.vs", resources_dir + "shaders/15.offscreen_rendering/construction.fs");
	Shader screenShader(resources_dir + "shaders/15.offscreen_rendering/screen.vs", resources_dir + "shaders/15.offscreen_rendering/screen.fs");

	Model constructionModel(resources_dir + "/models/construction-site-rawscan/site.obj");

	// set up vertex data and configure vertex attributes
	GLfloat vertices[] = {
		  // positions             // texture coordnates   // normals
		 -10.0f, -10.0f,  0.0f,    0.0f,  0.75f,  0.0f,    0.0f, 0.0f, 1.0f,
		 -5.0f,  -10.0f,  0.0f,    0.0f,  1.0f,   0.0f,    0.0f, 0.0f, 1.0f,
		  0.0f,  -10.0f,  0.0f,    0.25f, 0.75f,  0.0f,    0.0f, 0.0f, 1.0f,
		  5.0f,  -10.0f,  0.0f,    0.25f, 1.0f,   0.0f,    0.0f, 0.0f, 1.0f,
		  10.0f, -10.0f,  0.0f,    0.5f,  0.75f,  0.0f,    0.0f, 0.0f, 1.0f,
		 -10.0f, -5.0f,   0.0f,    0.5f,  1.0f,   0.0f,    0.0f, 0.0f, 1.0f,
		 -5.0f,  -5.0f,   0.0f,    0.75f, 0.75f,  0.0f,    0.0f, 0.0f, 1.0f,
		  0.0f,  -5.0f,   0.0f,    0.75f, 1.0f,   0.0f,    0.0f, 0.0f, 1.0f,
		  5.0f,  -5.0f,   0.0f,    1.0f,  0.75f,  0.0f,    0.0f, 0.0f, 1.0f,
		  10.0f, -5.0f,   0.0f,    1.0f,  1.0f,   0.0f,    0.0f, 0.0f, 1.0f,
		 -10.0f,  0.0f,   0.0f,    0.0f,  0.5f,   0.0f,    0.0f, 0.0f, 1.0f,
		 -5.0f,   0.0f,   0.0f,    0.25f, 0.5f,   0.0f,    0.0f, 0.0f, 1.0f,
		  0.0f,   0.0f,   0.0f,    0.5f,  0.5f,   0.0f,    0.0f, 0.0f, 1.0f,
		  5.0f,   0.0f,   0.0f,    0.75f, 0.5f,   0.0f,    0.0f, 0.0f, 1.0f,
		  10.0f,  0.0f,   0.0f,    1.0f,  0.5f,   0.0f,    0.0f, 0.0f, 1.0f,
		 -10.0f,  5.0f,   0.0f,    0.0f,  0.25f,  0.0f,    0.0f, 0.0f, 1.0f,
		 -5.0f,   5.0f,   0.0f,    0.25f, 0.25f,  0.0f,    0.0f, 0.0f, 1.0f,
		  0.0f,   5.0f,   0.0f,    0.5f,  0.25f,  0.0f,    0.0f, 0.0f, 1.0f,
		  5.0f,   5.0f,   0.0f,    0.75f, 0.25f,  0.0f,    0.0f, 0.0f, 1.0f,
		  10.0f,  5.0f,   0.0f,    1.0f,  0.25f,  0.0f,    0.0f, 0.0f, 1.0f,
		 -10.0f,  10.0f,  0.0f,    0.0f,  0.0f,   0.0f,    0.0f, 0.0f, 1.0f,
		 -5.0f,   10.0f,  0.0f,    0.25f, 0.0f,   0.0f,    0.0f, 0.0f, 1.0f,
		  0.0f,   10.0f,  0.0f,    0.5f,  0.0f,   0.0f,    0.0f, 0.0f, 1.0f,
		  5.0f,   10.0f,  0.0f,    0.75f, 0.0f,   0.0f,    0.0f, 0.0f, 1.0f,
		  10.0f,  10.0f,  0.0f,    1.0f,  0.0f,   0.0f,    0.0f, 0.0f, 1.0f
	};

	GLfloat indices[] = {
		6, 1, 6, 1, 2, 1, 7, 3, 7,
		2, 4, 2, 7, 3, 7, 1, 2, 1,
		7, 3, 7, 2, 4, 2, 8, 5, 8,
		3, 6, 3, 8, 5, 8, 2, 4, 2,
		8, 5, 8, 3, 6, 3, 9, 7, 9,
		4, 8, 4, 9, 7, 9, 3, 6, 3,
		9, 7, 9, 4, 8, 4, 10, 9, 10,
		5, 10, 5, 10, 9, 10, 4, 8, 4,
		11, 11, 11, 6, 1, 6, 12, 12, 12,
		7, 3, 7, 12, 12, 12, 6, 1, 6,
		12, 12, 12, 7, 3, 7, 13, 13, 13,
		8, 5, 8, 13, 13, 13, 7, 3, 7,
		13, 13, 13, 8, 5, 8, 14, 14, 14,
		9, 7, 9, 14, 14, 14, 8, 5, 8,
		14, 14, 14, 9, 7, 9, 15, 15, 15,
		10, 9, 10, 15, 15, 15, 9, 7, 9,
		16, 16, 16, 11, 11, 11, 17, 17, 17,
		12, 12, 12, 17, 17, 17, 11, 11, 11,
		17, 17, 17, 12, 12, 12, 18, 18, 18,
		13, 13, 13, 18, 18, 18, 12, 12, 12,
		18, 18, 18, 13, 13, 13, 19, 19, 19,
		14, 14, 14, 19, 19, 19, 13, 13, 13,
		19, 19, 19, 14, 14, 14, 20, 20, 20,
		15, 15, 15, 20, 20, 20, 14, 14, 14,
		21, 21, 21, 16, 16, 16, 22, 22, 22,
		17, 17, 17, 22, 22, 22, 16, 16, 16,
		22, 22, 22, 17, 17, 17, 23, 23, 23,
		18, 18, 18, 23, 23, 23, 17, 17, 17,
		23, 23, 23, 18, 18, 18, 24, 24, 24,
		19, 19, 19, 24, 24, 24, 18, 18, 18,
		24, 24, 24, 19, 19, 19, 25, 25, 25,
		20, 20, 20, 25, 25, 25, 19, 19, 19
	};
	
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastTime;
		lastTime = currentFrame;

		handleInput(window);

		// clear color and depth buffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		// swap buffers and poll IO events(keys pressed / released. mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void handleInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.handleKeyboard(Camera::CameraMovement::FORWARD, deltaTime);
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.handleKeyboard(Camera::CameraMovement::BACKWARD, deltaTime);
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.handleKeyboard(Camera::CameraMovement::LEFT, deltaTime);
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.handleKeyboard(Camera::CameraMovement::RIGHT, deltaTime);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}
	
	float xOffset = xPos - lastX;
	float yOffset = yPos - lastY;

	lastX = xPos;
	lastY = yPos;

	camera.handleMouseMovement(xOffset, -yOffset);
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.handleMouseScroll(yOffset);
}
