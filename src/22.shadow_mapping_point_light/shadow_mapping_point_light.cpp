
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Material> depthMapMat;

	Model* room;

	const uint16_t depthMapWidth = 2048;
	const uint16_t depthMapHeight = 2048;
	uint32_t depthMapFBO;
	uint32_t depthTexture;

	// point light position
	glm::vec3 lightPos = glm::vec3(0.0f, 3.0f, 0.0f);

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
			//{ Material::ShaderType::GEOMETRY, shadersDirectory + "depth_map.geom" },
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
	}

	virtual void render(float deltaTime) override
	{
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		room->setVector3("lightPos", lightPos);
		room->setVector3("viewPos", camera->getPosition());
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