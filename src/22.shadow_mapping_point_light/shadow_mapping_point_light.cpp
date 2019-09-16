
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Material> depthMapMat;
	
	Model* plane;
	Model* cube;

	const uint16_t depthMapWidth = 1024;
	const uint16_t depthMapHeight = 1024;
	uint32_t depthMapFBO;
	uint32_t depthTexture;

	Example()
	{
		title = "shadow mapping point light";
		settings.vsync = false;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/22.shadow_mapping_point_light/";
		texturesDirectory = getResourcesPath(ResourceType::Texture);
	}
	~Example()
	{
		delete(plane);
		delete(cube);
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		camera->movementSpeed = 2.0f;
		camera->rotationSpeed = 1.0f;
		camera->setPosition(glm::vec3(0.0f, 2.0f, 3.0f));
		camera->rotate(glm::vec3(45.0f, 0.0f, 0.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		std::unordered_map<Material::ShaderType, std::string> depthShaderPaths =
		{
			{ Material::ShaderType::VERTEX, shadersDirectory + "depth_map.vert" },
			{ Material::ShaderType::FRAGMENT, shadersDirectory + "depth_map.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> depthTexturePaths =
		{
		
		};

		std::unordered_map<Material::ShaderType, std::string> xenoShaderPaths =
		{
			{ Material::ShaderType::VERTEX, shadersDirectory + "xeno.vert" },
			{ Material::ShaderType::FRAGMENT, shadersDirectory + "xeno.frag" }
		};

		std::unordered_map<Material::ShaderType, std::string> planeShaderPaths =
		{
			{ Material::ShaderType::VERTEX, shadersDirectory + "plane.vert" },
			{ Material::ShaderType::FRAGMENT, shadersDirectory + "plane.frag" }
		};

		depthMapMat = std::make_shared<Material>(depthShaderPaths, depthTexturePaths);

		plane = Model::createWithFile(modelsDirectory + "/cube/cube.obj", planeShaderPaths);
		plane->setScale(glm::vec3(2.0f, 0.05f, 2.0f));

		cube = Model::createWithFile(modelsDirectory + "/cube/cube.obj", xenoShaderPaths);
		cube->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
		cube->setScale(glm::vec3(0.3f, 0.3f, 0.3f));

		// configure depth map FBO;
		glGenFramebuffers(1, &depthMapFBO);
		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depthMapWidth, depthMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		std::array<float, 4> borderColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor.data());
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapFBO, 0);
		glDrawBuffers(0, GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	virtual void render(float deltaTime) override
	{
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		World::getWorld()->enableGlobalMaterial(depthMapMat);
		glm::mat4 lightSpaceMatrix = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 100.0f) * glm::lookAt(glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0, 0.0, -1.0));
		depthMapMat->setMatrix4x4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, depthMapWidth, depthMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		depthMapMat->setMatrix4x4("model", cube->getModelMatrix());
		cube->render(deltaTime);
		depthMapMat->setMatrix4x4("model", plane->getModelMatrix());
		plane->render(deltaTime);
		World::getWorld()->disableGlobalMaterial();
		glCullFace(GL_BACK);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		cube->setInteger("depthMap", 0);
		plane->setInteger("depthMap", 0);

		cube->setMatrix4x4("lightSpaceMatrix", lightSpaceMatrix);
		plane->setMatrix4x4("lightSpaceMatrix", lightSpaceMatrix);

		cube->render(deltaTime);
		plane->render(deltaTime);
		
	}

	void renderQuad()
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		unsigned int quadVAO, quadVBO;
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
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