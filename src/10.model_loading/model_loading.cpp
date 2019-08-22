
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Example()
	{
		title = "model loading";
		settings.vsync = true;
		defaultClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/10.model_loading/";
		texturesDirectory = getResourcesPath(ResourceType::Texture) + "/10.model_loading/";
	}
	~Example()
	{
	
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		camera->rotationSpeed = 0.5f;
		camera->setPosition(glm::vec3(0.0f, 0.5f, 2.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		std::unordered_map<Material::ShaderType, std::string> shaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "model.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "model.frag" }
		};

		Model* model = Model::createWithFile(modelsDirectory + "/carnage/carnage.obj", shaderPaths);

		addObject(static_cast<Object*>(model));

		model->setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		model->setScale(glm::vec3(2.0f, 2.0f, 2.0f));
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