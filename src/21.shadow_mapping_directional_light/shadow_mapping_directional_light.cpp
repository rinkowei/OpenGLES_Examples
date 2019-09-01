
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Model* scene;

	Example()
	{
		title = "shadow mapping directional light";
		settings.vsync = false;
		settings.validation = true;
		defaultClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/21.shadow_mapping_directional_light/";
		texturesDirectory = getResourcesPath(ResourceType::Texture);
	}
	~Example()
	{
		delete(scene);
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		camera->movementSpeed = 2.0f;
		camera->rotationSpeed = 1.0f;
		camera->setPosition(glm::vec3(0.0f, 0.0f, 3.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		std::unordered_map<Material::ShaderType, std::string> depthShaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "depth_map.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "depth_map.frag" }
		};

		std::unordered_map<Material::ShaderType, std::string> sceneShaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "scene.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "scene.frag" }
		};

		scene = Model::createWithFile(modelsDirectory + "/susanoo/Susanoo.obj", sceneShaderPaths);
		scene->setScale(glm::vec3(0.1f, 0.1f, 0.1f));
	}

	virtual void render(float deltaTime) override
	{
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		scene->render(deltaTime);
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