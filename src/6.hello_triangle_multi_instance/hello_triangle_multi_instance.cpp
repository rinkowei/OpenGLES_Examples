
#define GLFW_INCLUDE_ES32
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Common/shader.h>
#include <iostream>
#include <array>
using namespace std;

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void handleInput(GLFWwindow* window);

const GLuint SCREEN_WIDTH = 800;
const GLuint SCREEN_HEIGHT = 600;

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

	if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD\n";
		return -1;
	}
	else
	{
		cout << "Success to initialize GLAD\n";
	}

	string resources_dir(ES_EXAMPLE_RESOURCES_DIR);

	Shader shader(resources_dir + "shaders/6.hello_triangle_multi_instance/triangle.vs", resources_dir + "shaders/6.hello_triangle_multi_instance/triangle.fs");

	// create a array of 100 triangle locations
	std::array<glm::vec2, 100> locations;
	unsigned int index = 0;
	float offset = 0.1f;
	for (int x = -10; x < 10; x += 2)
	{
		for (int y = -10; y < 10; y += 2)
		{
			glm::vec2 position((float)x / 10.0f + offset, (float)y / 10.0f + offset);
			locations[index++] = position;
		}
	}

	// store instance data into an array buffer
	GLuint instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * locations.size(), locations.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLfloat vertices[] = {
		// positions           // colors
		 0.05f, -0.05f, 0.0f,   1.0f, 0.0f, 0.0f, 1.0f,  
		-0.05f, -0.05f, 0.0f,   0.0f, 1.0f, 0.0f, 1.0f,  
		-0.05f,  0.05f, 0.0f,   0.0f, 0.0f, 1.0f, 1.0f,
	};
	GLuint indices[] = {
		0, 1, 2
	};
	
	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		handleInput(window);
		
		// clear color buffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.use();
		glBindVertexArray(VAO);
		glDrawElementsInstanced(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0, 100);

		// swap buffers and poll IO events(keys pressed / released. mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	
	glfwTerminate();
	return 0;
}

void handleInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
