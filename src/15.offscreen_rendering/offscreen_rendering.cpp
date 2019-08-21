/*
#define GLFW_INCLUDE_ES32
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common.h>

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

GLuint quadVAO = 0;
GLuint quadVBO = 0;
GLuint quadEBO = 0;

void drawQuad()
{
	if (quadVAO == 0)
	{
		GLfloat vertices[] = {
			 // positions        // texture coordinates
			 0.4f, 1.3f, 0.0f,  1.0f, 1.0f,
			 0.4f, 0.3f, 0.0f,  1.0f, 0.0f,
			-0.4f, 0.3f, 0.0f,  0.0f, 0.0f,
			-0.4f, 1.3f, 0.0f,  0.0f, 1.0f
		};

		GLuint indices[] = {
			0, 1, 3,
			1, 2, 3
		};

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glGenBuffers(1, &quadEBO);

		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

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
	
	Shader* constructionShader = Shader::createWithFile(resources_dir + "shaders/15.offscreen_rendering/construction.vs", resources_dir + "shaders/15.offscreen_rendering/construction.fs");
	Shader* screenShader = Shader::createWithFile(resources_dir + "shaders/15.offscreen_rendering/screen.vs", resources_dir + "shaders/15.offscreen_rendering/screen.fs");

	Model constructionModel(resources_dir + "/models/construction-site-rawscan/site.obj");

	// configure framebuffer
	GLuint framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	GLuint colorAttachment;
	glGenTextures(1, &colorAttachment);
	glBindTexture(GL_TEXTURE_2D, colorAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);

	GLuint renderbuffer;
	glGenRenderbuffers(1, &renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("failed to create framebuffer!");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastTime;
		lastTime = currentFrame;

		handleInput(window);
		
		// switch to another framebuffer we have created, so now would not render to screen
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST);

		// clear color and depth buffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		constructionShader->use();
		constructionShader->setMat4("view", camera.getViewMatrix());
		constructionShader->setMat4("projection", camera.getPerspectiveMatrix());

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		constructionShader->setMat4("model", model);

		constructionModel.Draw(*constructionShader);

		// switch to default famebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// clear color and depth buffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		constructionModel.Draw(*constructionShader);

		// disable depth test for render quad in front of the scene
		glDisable(GL_DEPTH_TEST);

		screenShader->use();
		screenShader->setInt("screenTexture", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorAttachment);
		drawQuad();
		
		// swap buffers and poll IO events(keys pressed / released. mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// delete all buffers and VAOs
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteBuffers(1, &quadEBO);

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
*/


#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Example()
	{
		title = "offscreen rendering";
		settings.vsync = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/15.offscreen_rendering/";
	}
	~Example()
	{

	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		camera->movementSpeed = 1.0f;
		camera->rotationSpeed = 0.5f;
		camera->setPosition(glm::vec3(0.0f, 0.0f, 4.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		std::vector<GLfloat> vertexAttrs = {
			// positions         // texture coordinates
			0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
			0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
			1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

			0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
			1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
			1.0f,  0.5f,  0.0f,  1.0f,  0.0f
		};

		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexAttrs.size() / 8); i++)
		{
			Vertex vertex;
			vertex.Position = glm::vec3(vertexAttrs[i * 5], vertexAttrs[i * 5 + 1], vertexAttrs[i * 5 + 2]);
			vertex.TexCoords = glm::vec2(vertexAttrs[i * 5 + 3], vertexAttrs[i * 5 + 4]);
			vertices.push_back(vertex);
		}

		std::unordered_map<Material::ShaderType, std::string> shaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "construction.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "construction.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> texturePaths =
		{

		};

		// create quad material
		std::shared_ptr<Material> material = std::make_shared<Material>(shaderPaths, texturePaths);

		Framebuffer* framebuffer = Framebuffer::create(width, height, (uint32_t)Framebuffer::AttachmentType::ColorBuffer_1 | (uint32_t)Framebuffer::AttachmentType::RenderBuffer);
		
		Model* model = Model::createWithFile(modelsDirectory + "/construction-site-rawscan/site.obj", shaderPaths);

		//addObject(static_cast<Object*>(model));
	}

	virtual void update(float deltaTime) override
	{
		
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