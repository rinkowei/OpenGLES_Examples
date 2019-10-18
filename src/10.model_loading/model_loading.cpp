#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> model;
	std::shared_ptr<Material> mat;

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

		mat = Material::createFromFiles("model_mat",
			{
				shadersDirectory + "model.vert",
				shadersDirectory + "model.frag"
			},
			{
				{ "diffuseMap_0", modelsDirectory + "/nanosuit/body_dif.png" }
			}
		);

		model = Model::createFromFile("nanosuit", modelsDirectory + "/nanosuit/nanosuit.obj");
		model->setMaterial(mat);
		model->setPosition(glm::vec3(0.0f));
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &destWidth, &destHeight);
		glViewport(0, 0, destWidth, destHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//mat->apply();
		//mat->setUniform("model", glm::mat4(1.0f));
		//mat->setUniform("view", mMainCamera->getView());
		//mat->setUniform("projection", mMainCamera->getProjection());
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