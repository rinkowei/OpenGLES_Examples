#include <examplebase.h>
#include <model.h>
#include <material.h>
#include <buffer.h>
#include <random>
#include <ctime>
using namespace es;

class Example final : public ExampleBase
{
public:
	const int row = 7;
	const int col = 7;
	std::vector<std::shared_ptr<Model>> spheres;

	struct Light
	{
		glm::vec3 color;
		glm::vec3 position;
	};
	std::array<Light, 4> lights;

	Example()
	{
		title = "image based lighting pbr";
		settings.vsync = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/28.image_based_lighting_pbr/";
	}

	~Example()
	{
		spheres.swap(std::vector<std::shared_ptr<Model>>());
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		mMainCamera->setPosition(glm::vec3(0.0f, 10.0f, 12.0f));
		mMainCamera->setRotation(glm::vec3(45.0f, 0.0f, 0.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		std::shared_ptr<Model> sphereTemplate = Model::createFromFile("sphere_template", modelsDirectory + "/sphere/sphere.obj",
			{
				shadersDirectory + "pbr.vert",
				shadersDirectory + "pbr.frag"
			},
			true
		);

		lights[0].position = glm::vec3(-15.0f, 7.5f, -15.0f);
		lights[1].position = glm::vec3(-15.0f, 7.5f, 15.0f);
		lights[2].position = glm::vec3(15.0f, 7.5f, 15.0f);
		lights[3].position = glm::vec3(15.0f, 7.5f, -15.0f);
		
		for (int x = 0; x < row; x++)
		{
			for (int y = 0; y < col; y++)
			{
				std::shared_ptr<Model> sphere = Model::clone("sphere_" + std::to_string(x * col + y), sphereTemplate.get());
			
				glm::vec3 pos = glm::vec3(float(y - (row / 2.0f)) * 2.5f, 0.0f, float(x - (col / 2.0f)) * 2.5f);
				sphere->setPosition(pos);
				sphere->setScale(glm::vec3(0.035f));
				sphere->setUniform("albedo", glm::vec3(1.0f, 0.765557f, 0.336057f));
				sphere->setUniform("roughness", glm::clamp((float)x / (float)(row - 1), 0.05f, 1.0f));
				sphere->setUniform("metallic", glm::clamp((float)col / (float)(y + 1), 0.1f, 1.0f));
				sphere->setUniform("ao", 1.0f);
				sphere->setUniform("exposure", 1.0f);

				for (std::size_t i = 0; i < lights.size(); i++)
				{
					lights[i].color = glm::vec3(1.0f, 1.0f, 1.0f);
					sphere->setUniform("lights[" + std::to_string(i) + "].position", lights[i].position);
					sphere->setUniform("lights[" + std::to_string(i) + "].color", lights[i].color);
				}

				spheres.push_back(sphere);
			}
		}
	}

	virtual void render(float deltaTime) override
	{
		for (std::size_t i = 0; i < spheres.size(); i++)
		{
			spheres[i]->setUniform("viewPos", mMainCamera->getPosition());
			spheres[i]->render();
		}
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