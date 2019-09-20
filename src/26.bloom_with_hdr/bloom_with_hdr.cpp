
#include <common.h>
#include <random>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::array<Model*, 16> spheres;

	glm::vec3 lightPos = glm::vec3(0.0f, 5.0f, 0.0f);

	Example()
	{
		title = "bloom with hdr";
		settings.vsync = false;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/26.bloom_with_hdr/";
	}

	~Example()
	{
		for (size_t i = 0; i < spheres.size(); i++)
		{
			delete(spheres[i]);
		}
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		camera->movementSpeed = 2.0f;
		camera->rotationSpeed = 1.0f;
		camera->setPosition(glm::vec3(0.0f, 0.0f, 10.0f));
		camera->rotate(glm::vec3(0.0f, 0.0f, 0.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		// enable cull face
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);

		std::unordered_map<Material::ShaderType, std::string> hdrShaderPaths =
		{
			{ Material::ShaderType::VERTEX, shadersDirectory + "hdr.vert" },
			{ Material::ShaderType::FRAGMENT, shadersDirectory + "hdr.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> hdrTexturePaths =
		{

		};

		std::shared_ptr<Material> sphereMat = std::make_shared<Material>(hdrShaderPaths, hdrTexturePaths);

		for (size_t i = 0; i < spheres.size(); i++)
		{
			Model* sphere = Model::createWithFile(modelsDirectory + "/sphere/sphere.obj", sphereMat);

			default_random_engine e(glfwGetTime());
			uniform_real_distribution<double> u(0.0, 1.0);
			float x = u(e);
			float y = u(e);
			float z = u(e);
			u.reset();
			glm::vec3 randomColor = glm::vec3(x, y, z);
			sphere->setVector3("randomColor", randomColor);

			glm::vec3 pos = glm::vec3(sin(glm::radians(i * (360.0f / spheres.size()))), cos(glm::radians(i * (360.0f / spheres.size()))), 0.0f) * 3.5f;
			sphere->setPosition(pos);
			sphere->setScale(glm::vec3(0.02f));
			spheres[i] = sphere;
		}
	}

	virtual void render(float deltaTime) override
	{
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		for (size_t i = 0; i < spheres.size(); i++)
		{
			spheres[i]->render(deltaTime);
		}
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