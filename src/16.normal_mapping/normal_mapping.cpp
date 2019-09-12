
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Model* planeModel;

	Example()
	{
		title = "normal mapping";
		settings.vsync = false;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/16.normal_mapping/";
	}
	~Example()
	{
		delete(planeModel);
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		camera->movementSpeed = 2.0f;
		camera->rotationSpeed = 0.5f;
		camera->setPosition(glm::vec3(0.0f, 2.0f, 2.0f));
		camera->setRotation(glm::vec3(45.0f, 0.0f, 0.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		std::unordered_map<Material::ShaderType, std::string> shaderPaths =
		{
			{ Material::ShaderType::VERTEX, shadersDirectory + "normal.vert" },
			{ Material::ShaderType::FRAGMENT, shadersDirectory + "normal.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> texturePaths =
		{
			std::make_pair(Texture::Type::DIFFUSE, modelsDirectory + "/rocks_plane/rocks_color_bc3_unorm.png"),
			std::make_pair(Texture::Type::NORMAL, modelsDirectory + "/rocks_plane/rocks_normal_height_rgba.png")
		};

		std::shared_ptr<Material> material = std::make_shared<Material>(shaderPaths, texturePaths);

		planeModel = Model::createWithFile(modelsDirectory + "/rocks_plane/rocks_plane.obj", material);
		planeModel->setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		planeModel->setScale(glm::vec3(0.2f, 0.2f, 0.2f));
	}

	virtual void render(float deltaTime) override
	{
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		planeModel->setVector3("lightPos", glm::vec3(sin(glm::radians(timePassed * 360.0f)) * 1.5f, 5.0f, cos(glm::radians(timePassed * 360.0f)) * 1.5f));
		planeModel->setVector3("viewPos", camera->getPosition());
		planeModel->render(deltaTime);
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