#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> planeModel;

	Example()
	{
		title = "normal mapping";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/16.normal_mapping/";
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

		mMainCamera->setPosition(glm::vec3(0.0f, 5.0f, -5.0f));

		std::shared_ptr<Material> mat = Material::createFromFiles("mat",
			{
				shadersDirectory + "normal.vert",
				shadersDirectory + "normal.frag"
			},
			{
				{ "diffuseMap_0", modelsDirectory + "/rocks_plane/rocks_color_bc3_unorm.png" },
				{ "normalMap_0", modelsDirectory + "/rocks_plane/rocks_normal_height_rgba.png" }
			}
		);

		planeModel = Model::createFromFile("rocks_plane",
			modelsDirectory + "/rocks_plane/rocks_plane.obj",
			{

			},
			false
		);

		planeModel->setMaterial(mat);
		planeModel->setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &destWidth, &destHeight);
		glViewport(0, 0, destWidth, destHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		planeModel->setUniform("lightPos", glm::vec3(sin(glm::radians(timePassed * 360.0f)) * 1.5f, 5.0f, cos(glm::radians(timePassed * 360.0f)) * 1.5f));
		planeModel->setUniform("viewPos", mMainCamera->getPosition());
		planeModel->render();
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