/*
#define GLFW_INCLUDE_ES32
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <common.h>
#include <iostream>
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

	Shader* shader = Shader::createWithFile(resources_dir + "shaders/5.hello_triangle_restart_index/triangle.vs", resources_dir + "shaders/5.hello_triangle_restart_index/triangle.fs");
	GLfloat vertices[] = {
		-0.6f, -0.3f, 0.0f,
		-0.3f, 0.4f, 0.0f,
		0.0f, -0.3f, 0.0f,
		0.3f, 0.4f, 0.0f,
		0.6f, -0.3f, 0.0f,
	};
	GLuint indices[] = {
		0, 1, 2,
		0xFFFF,    // restart index
		2, 3, 4
	};

	// enable primitive restart
	glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	
	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		handleInput(window);
		
		// clear color buffer
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader->use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLE_STRIP, 7, GL_UNSIGNED_INT, 0);
		
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
*/


#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Mesh* triangle;
	Shader* shader;
	Example()
	{
		title = "triangle restart index";
		defaultClearColor = glm::vec4(0.40f, 0.40f, 0.50f, 1.0f);

		shadersPath = getResourcesPath(ResourceType::Shader) + "/05.triangle_restart_index/";
	}
	~Example()
	{
		delete(shader);
		delete(triangle);
	}
public:
	virtual void prepare() override
	{
		std::vector<GLfloat> vertexPositions = {
			-0.6f, -0.3f, 0.0f,
			-0.3f, 0.4f, 0.0f,
			0.0f, -0.3f, 0.0f,
			0.3f, 0.4f, 0.0f,
			0.6f, -0.3f, 0.0f
		};

		std::vector<GLuint> indices = {
			0, 1, 2,
			0xFFFF,    // restart index
			2, 3, 4
		};

		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexPositions.size() / 3); i++)
		{
			Vertex vertex;
			vertex.Position = glm::vec3(vertexPositions[i * 3], vertexPositions[i * 3 + 1], vertexPositions[i * 3 + 2]);
			vertices.push_back(vertex);
		}

		// create triangle mesh
		triangle = Mesh::createWithData(vertices, indices, {}, Mesh::DrawType::Elements_Restart_Index);

		// create triangle shader
		shader = Shader::createWithFile(shadersPath + "triangle.vs", shadersPath + "triangle.fs");
	}
	virtual void render() override
	{
		// render mesh
		triangle->Draw(shader);
	}
};

Example* example;
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	example = new Example();
	example->setupValidation();
	if (!example->setupGLFW() ||
		!example->loadGLESFunctions() ||
		!example->setupImGui())
	{
		return 0;
	}
	example->prepare();
	example->renderLoop();
	delete(example);
	return 0;
}



