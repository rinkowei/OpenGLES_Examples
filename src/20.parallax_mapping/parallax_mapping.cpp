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
		title = "parallax mapping";
		settings.vsync = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/20.parallax_mapping/";
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

		mMainCamera->setPosition(glm::vec3(0.0f, 0.0f, 25.0f));
		mMainCamera->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));

		std::shared_ptr<Material> mat = Material::createFromFiles("mat",
			{
				shadersDirectory + "parallax.vert",
				shadersDirectory + "parallax.frag"
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

		planeModel->setUniform("numLayers", 48.0f);
		planeModel->setUniform("heightScale", 0.1f);
		planeModel->setUniform("parallaxBias", -0.02f);
	}

	virtual void render(float deltaTime) override
	{
		planeModel->setUniform("lightPos", glm::vec3(sin(glm::radians(timePassed * 360.0f)) * 1.5f, -3.0f, cos(glm::radians(timePassed * 360.0f)) * 3.5f + 5.0f));
		planeModel->setUniform("viewPos", mMainCamera->getPosition());
		planeModel->render();
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