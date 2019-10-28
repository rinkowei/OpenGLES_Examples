#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Model* skybox;
	Model* sphere;

	Example()
	{
		title = "skybox reflect";
		settings.vsync = false;
		defaultClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/19.skybox_reflect/";
		texturesDirectory = getResourcesPath(ResourceType::Texture);
	}
	~Example()
	{
		delete(skybox);
		delete(sphere);
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		std::unordered_map<Material::ShaderType, std::string> skyboxShaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "skybox.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "skybox.frag" }
		};

		std::unordered_map<Material::ShaderType, std::string> sphereShaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "reflect.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "reflect.frag" }
		};

		// create a sphere model
		sphere = Model::createWithFile(modelsDirectory + "/sphere/sphere.obj", sphereShaderPaths);
		sphere->setScale(glm::vec3(0.03f, 0.03f, 0.03f));
		// create a cube model as skybox
		skybox = Model::createWithFile(modelsDirectory + "/cube/cube.obj", skyboxShaderPaths);

		TextureCube* cubemap = TextureCube::createWithFiles({texturesDirectory + "/skyboxes/sincity/right.tga", texturesDirectory + "/skyboxes/sincity/left.tga",
															 texturesDirectory + "/skyboxes/sincity/top.tga", texturesDirectory + "/skyboxes/sincity/bottom.tga",
															 texturesDirectory + "/skyboxes/sincity/front.tga", texturesDirectory + "/skyboxes/sincity/back.tga"});
		
		sphere->setInteger("skybox", 0);
		skybox->setInteger("cubemap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->getID());
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &destWidth, &destHeight);
		glViewport(0, 0, destWidth, destHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		sphere->render();

		// change depth function so depth test passes when depth values are equal to content of depth buffer
		glDepthFunc(GL_LEQUAL);
		// render skybox at last
		skybox->render();
		// set depth function back to default
		glDepthFunc(GL_LESS);
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