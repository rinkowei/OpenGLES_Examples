
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Material> depthMapMat;

	Model* room;

	const uint16_t depthMapWidth = 1024;
	const uint16_t depthMapHeight = 1024;
	uint32_t depthMapFBO;
	uint32_t depthCubeMap;

	// point light position
	glm::vec3 lightPos = glm::vec3(0.0f, 2.5f, 0.0f);
	// point light matrices
	std::vector<glm::mat4> lightMatrices;

	float nearPlane = 1.0f;
	float farPlane = 10.0f;

	Example()
	{
		title = "shadow mapping point light";
		settings.vsync = false;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/22.shadow_mapping_point_light/";
	}

	~Example()
	{
		delete(room);
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
			{ Material::ShaderType::GEOMETRY, shadersDirectory + "depth_map.geom" },
			{ Material::ShaderType::FRAGMENT, shadersDirectory + "depth_map.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> depthTexturePaths =
		{

		};

		depthMapMat = std::make_shared<Material>(depthShaderPaths, depthTexturePaths);

		std::unordered_map<Material::ShaderType, std::string> roomShaderPaths =
		{
			{ Material::ShaderType::VERTEX, shadersDirectory + "room.vert" },
			{ Material::ShaderType::FRAGMENT, shadersDirectory + "room.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> roomTexturePaths =
		{
			
		};

		std::shared_ptr<Material> roomMat = std::make_shared<Material>(roomShaderPaths, roomTexturePaths);

		room = Model::createWithFile(modelsDirectory + "/van-gogh-room/van-gogh-room.obj", roomMat);

		// configure depth map FBO
		glGenFramebuffers(1, &depthMapFBO);
		glGenTextures(1, &depthCubeMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
		for (unsigned int i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, depthMapWidth, depthMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
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

		glm::mat4 lightProj = glm::perspective(glm::radians(90.0f), (float)depthMapWidth / (float)depthMapHeight, nearPlane, farPlane);
		lightMatrices.push_back(lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		lightMatrices.push_back(lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		lightMatrices.push_back(lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		lightMatrices.push_back(lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		lightMatrices.push_back(lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		lightMatrices.push_back(lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		glViewport(0, 0, depthMapWidth, depthMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		World::getWorld()->enableGlobalMaterial(depthMapMat);
		glCullFace(GL_FRONT);
		for (unsigned int i = 0; i < 6; ++i)
			depthMapMat->setMatrix4x4("lightMatrices[" + std::to_string(i) + "]", lightMatrices[i]);
		depthMapMat->setFloat("farPlane", farPlane);
		depthMapMat->setVector3("lightPos", lightPos);
		depthMapMat->setMatrix4x4("model", room->getModelMatrix());
		room->render(deltaTime);
		glCullFace(GL_BACK);
		World::getWorld()->disableGlobalMaterial();
		
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		room->setFloat("farPlane", farPlane);
		room->setVector3("lightPos", lightPos);
		room->setVector3("viewPos", camera->getPosition());
		room->setInteger("depthMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
		room->render(deltaTime);
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