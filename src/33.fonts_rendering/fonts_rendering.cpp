#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> spider1;
	std::shared_ptr<Model> spider2;

	glm::vec3 lightPos = glm::vec3(0.0f, 10.0f, 0.0f);

	Example()
	{
		title = "geometry shader billboard";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/33.geometry_shader_billboard/";
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

		spider1 = Model::createFromFile("spider_1", modelsDirectory + "/spider/spider.obj",
			{
				{ shadersDirectory + "lighting.vert" },
				{ shadersDirectory + "lighting.frag" },
			},
			true
		);
		spider1->setScale(glm::vec3(0.01f));
		spider1->setUniform("lightPos", lightPos);
	}

	virtual void render(float deltaTime) override
	{
		
		spider1->setUniform("viewPos", mMainCamera->getPosition());
		spider1->render();
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