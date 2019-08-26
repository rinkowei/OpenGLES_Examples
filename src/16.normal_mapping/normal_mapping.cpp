
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Model* planeModel;
	std::vector<Mesh*> meshes;

	Example()
	{
		title = "normal mapping";
		settings.vsync = false;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/16.normal_mapping/";
		texturesDirectory = getResourcesPath(ResourceType::Texture) + "/16.normal_mapping/";
	}
	~Example()
	{
		delete(planeModel);
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		camera->rotationSpeed = 0.5f;
		camera->setPosition(glm::vec3(0.0f, 3.0f, 4.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		std::unordered_map<Material::ShaderType, std::string> shaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "plane.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "plane.frag" }
		};

		planeModel = Model::createWithFile(modelsDirectory + "/Aussie-Telco/grid.obj", shaderPaths);

		Texture* diffuseMap = Texture::createWithFile(modelsDirectory + "/Aussie-Telco/telstra_COL.jpg", Texture::Type::Diffuse);
		Texture* specularMap = Texture::createWithFile(modelsDirectory + "/Aussie-Telco/telstra_DSP.jpg", Texture::Type::Specular);
		Texture* normalMap = Texture::createWithFile(modelsDirectory + "/Aussie-Telco/telstra_NRM.jpg", Texture::Type::Normal);

		meshes = planeModel->getMeshes();
		for (size_t i = 0; i < meshes.size(); i++)
		{
			meshes[i]->getMaterial()->setVec3("lightPos", glm::vec3(0.0f, 2.0f, 0.0f));
			meshes[i]->getMaterial()->setInt("diffuseMap_0", 0);
			meshes[i]->getMaterial()->setInt("specularMap_0", 1);
			meshes[i]->getMaterial()->setInt("normalMap_0", 2);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap->getID());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap->getID());
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, normalMap->getID());
	}

	virtual void render(float deltaTime) override
	{
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (size_t i = 0; i < meshes.size(); i++)
		{
			meshes[i]->getMaterial()->setVec3("viewPos", camera->getPosition());
		}
		planeModel->render(deltaTime);
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