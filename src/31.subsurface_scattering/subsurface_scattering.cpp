#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> bunny;

	std::unique_ptr<Framebuffer> depthFBO;

	Example()
	{
		title = "subsurface scattering";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/31.subsurface_scattering/";
		texturesDirectory = getResourcesPath(ResourceType::Texture);
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
	}

	virtual void render(float deltaTime) override
	{
		bunny = Model::createFromFile("bunny", modelsDirectory + "/bunny/bunny.obj", {}, false);
	}

	virtual void windowResized() override
	{
		ExampleBase::windowResized();
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