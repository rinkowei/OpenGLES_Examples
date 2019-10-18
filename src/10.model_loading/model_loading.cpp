#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> model;

	Example()
	{
		title = "model loading";
		settings.vsync = true;
		settings.validation = true;
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

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		model = Model::createFromFile("nanosuit", 
			modelsDirectory + "/nanosuit/nanosuit.obj", 
			{
				shadersDirectory + "model.vert",
				shadersDirectory + "model.frag"
			}
		);

		model->setPosition(glm::vec3(0.0f, -1.5f, 0.0f));
		model->setScale(glm::vec3(0.2f));
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &destWidth, &destHeight);
		glViewport(0, 0, destWidth, destHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		model->render();
	}
};

Example* example;
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	example = new Example();
	example->setupValidation();
	if (!example->setupSDL() ||
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