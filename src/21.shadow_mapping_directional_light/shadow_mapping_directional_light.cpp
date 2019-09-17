
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Material> depthMapMat;
	
	Model* playground;

	const uint16_t depthMapWidth = 2048;
	const uint16_t depthMapHeight = 2048;
	uint32_t depthMapFBO;
	uint32_t depthTexture;

	glm::vec3 lightPos = glm::vec3(-2.0f, 5.0f, 0.0f);

	glm::mat4 biasMatrix = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	);

	Example()
	{
		title = "shadow mapping directional light";
		settings.vsync = false;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/21.shadow_mapping_directional_light/";
		texturesDirectory = getResourcesPath(ResourceType::Texture);
	}
	~Example()
	{
		delete(playground);
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

		// enable cull face
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);
		glCullFace(GL_BACK);

		std::unordered_map<Material::ShaderType, std::string> depthShaderPaths =
		{
			{ Material::ShaderType::VERTEX, shadersDirectory + "depth_map.vert" },
			{ Material::ShaderType::FRAGMENT, shadersDirectory + "depth_map.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> depthTexturePaths =
		{
		
		};

		depthMapMat = std::make_shared<Material>(depthShaderPaths, depthTexturePaths);

		std::unordered_map<Material::ShaderType, std::string> groundShaderPaths =
		{
			{ Material::ShaderType::VERTEX, shadersDirectory + "playground.vert" },
			{ Material::ShaderType::FRAGMENT, shadersDirectory + "playground.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> groundTexturePaths =
		{
			
		};

		std::shared_ptr<Material> groundMat = std::make_shared<Material>(groundShaderPaths, groundTexturePaths);

		playground = Model::createWithFile(modelsDirectory + "/playground/Playground.obj", groundMat);

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

		// change light position over time
		lightPos = glm::vec3(sin(glfwGetTime()) * 3.0f, 5.0f + cos(glfwGetTime()) * 1.0f, cos(glfwGetTime()) * 2.0f);
		glm::mat4 lightProj = glm::ortho<float>(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 10.0f);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProj * lightView;
		depthMapMat->setMatrix4x4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, depthMapWidth, depthMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// fixed peter panning, use cull front face when render scene to depth map
		glCullFace(GL_FRONT);
		depthMapMat->setMatrix4x4("model", playground->getModelMatrix());
		playground->render(deltaTime);
		glCullFace(GL_BACK);
		World::getWorld()->disableGlobalMaterial();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		playground->setInteger("depthMap", 0);

		playground->setVector3("lightPos", lightPos);
		playground->setVector3("viewPos", camera->getPosition());
		playground->setMatrix4x4("lightSpaceMatrix", lightSpaceMatrix);
		playground->setMatrix4x4("biasMatrix", biasMatrix);
		playground->render(deltaTime);
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