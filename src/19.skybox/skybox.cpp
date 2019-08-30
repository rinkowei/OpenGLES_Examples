
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Model* skybox;

	Example()
	{
		title = "skybox";
		settings.vsync = false;
		settings.validation = true;
		defaultClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/19.skybox/";
		texturesDirectory = getResourcesPath(ResourceType::Texture);
	}
	~Example()
	{
		delete(skybox);
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		camera->movementSpeed = 2.0f;
		camera->rotationSpeed = 1.0f;
		camera->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		std::vector<GLfloat> vertexAttrs = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexAttrs.size() / 3); i++)
		{
			Vertex vertex;
			vertex.Position = glm::vec3(vertexAttrs[i * 3], vertexAttrs[i * 3 + 1], vertexAttrs[i * 3 + 2]);
			vertices.push_back(vertex);
		}

		std::unordered_map<Material::ShaderType, std::string> skyboxShaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "skybox.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "skybox.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> texturePaths =
		{
			
		};

		std::shared_ptr<Material> material = std::make_shared<Material>(skyboxShaderPaths, texturePaths);

		// create a cube model as skybox
		skybox = Model::createWithFile(modelsDirectory + "/cube/cube.obj", skyboxShaderPaths);

		TextureCube* cubemap = TextureCube::createWithFiles({texturesDirectory + "/skyboxes/sincity/right.tga", texturesDirectory + "/skyboxes/sincity/left.tga",
															 texturesDirectory + "/skyboxes/sincity/top.tga", texturesDirectory + "/skyboxes/sincity/bottom.tga",
															 texturesDirectory + "/skyboxes/sincity/front.tga", texturesDirectory + "/skyboxes/sincity/back.tga"});
		skybox->setInteger("cubemap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->getID());
	}

	virtual void render(float deltaTime) override
	{
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		glDepthFunc(GL_LEQUAL);
		skybox->render(deltaTime);
		glDepthFunc(GL_LESS);
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