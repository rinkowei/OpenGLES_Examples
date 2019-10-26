#include <examplebase.h>
#include <model.h>
#include <material.h>
#include <random>
#include <ctime>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::array<std::shared_ptr<Model>, 16> spheres;

	glm::vec3 lightPos = glm::vec3(0.0f, 5.0f, 0.0f);

	Example()
	{
		title = "bloom with hdr";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/26.bloom_with_hdr/";
	}

	~Example()
	{
		spheres.swap(std::array<std::shared_ptr<Model>, 16>());
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		mMainCamera->setPosition(glm::vec3(0.0f, 0.0f, 10.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		// enable cull face
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		std::shared_ptr<Material> mat = Material::createFromFiles("mat",
			{
				shadersDirectory + "hdr.vert",
				shadersDirectory + "hdr.frag"
			},
			{
	
			}
		);

		std::default_random_engine e(time(0));

		for (size_t i = 0; i < spheres.size(); i++)
		{
			std::shared_ptr<Model> sphere = Model::createFromFile("sphere_" + std::to_string(i), modelsDirectory + "/sphere/sphere.obj", 
				{
					shadersDirectory + "hdr.vert",
					shadersDirectory + "hdr.frag"
				},
				true
			);
			//sphere->setUniform("randomColor", glm::vec3(1.0f, 0.0f, 0.0f));
			
			//std::default_random_engine e(SDL_GetTicks());
			std::uniform_real_distribution<double> u(0.0, 1.0);
			float x = u(e);
			float y = u(e);
			float z = u(e);
			u.reset();
			glm::vec3 randomColor = glm::vec3(x, y, z);
			sphere->setUniform("randomColor", randomColor);
			
			glm::vec3 pos = glm::vec3(sin(glm::radians(i * (360.0f / spheres.size()))), cos(glm::radians(i * (360.0f / spheres.size()))), 0.0f) * 3.5f;
			sphere->setPosition(pos);
			sphere->setScale(glm::vec3(0.02f));
			spheres[i] = sphere;
		}
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &destWidth, &destHeight);
		glViewport(0, 0, destWidth, destHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		for (size_t i = 0; i < spheres.size(); i++)
		{
			spheres[i]->render();
		}
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