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
	std::array<std::shared_ptr<Model>, 16> spheres;
	std::unique_ptr<Framebuffer> hdrFBO;

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
		std::uniform_real_distribution<double> u(1.5f, 15.0f);

		std::shared_ptr<Model> sphereTemplate = Model::createFromFile("sphere_template", modelsDirectory + "/sphere/sphere.obj",
			{
				shadersDirectory + "hdr.vert",
				shadersDirectory + "hdr.frag"
			},
			true
		);
		
		for (size_t i = 0; i < spheres.size(); i++)
		{
			std::shared_ptr<Model> sphere = Model::clone("sphere_" + std::to_string(i), sphereTemplate.get());

			glm::vec3 randomColor = glm::vec3(u(e), u(e), u(e));
			sphere->setUniform("randomColor", randomColor);
			
			glm::vec3 pos = glm::vec3(sin(glm::radians(i * (360.0f / spheres.size()))), cos(glm::radians(i * (360.0f / spheres.size()))), 0.0f) * 3.5f;
			sphere->setPosition(pos);
			sphere->setScale(glm::vec3(0.02f));
			spheres[i] = sphere;
		}
		
		hdrFBO = Framebuffer::create();

		std::shared_ptr<Texture2D> fragColorTexture = Texture2D::createFromData(defaultWindowWidth, defaultWindowHeight, 1, 1, 1, GL_RGB16F, GL_RGB, GL_FLOAT);
		fragColorTexture->setMinFilter(GL_LINEAR);
		fragColorTexture->setMagFilter(GL_LINEAR);
		fragColorTexture->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		std::shared_ptr<Texture2D> brightColorTexture = Texture2D::createFromData(defaultWindowWidth, defaultWindowHeight, 1, 1, 1, GL_RGB16F, GL_RGB, GL_FLOAT);
		brightColorTexture->setMinFilter(GL_LINEAR);
		brightColorTexture->setMagFilter(GL_LINEAR);
		brightColorTexture->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		hdrFBO->attachRenderTarget(0, fragColorTexture.get(), 0, 0);
		hdrFBO->attachRenderTarget(1, brightColorTexture.get(), 0, 0);
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