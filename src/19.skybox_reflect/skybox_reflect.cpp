#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> skybox;

	Example()
	{
		title = "skybox reflect";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/19.skybox_reflect/";
		texturesDirectory = getResourcesPath(ResourceType::Texture);
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

		std::shared_ptr<TextureCube> cubemap = TextureCube::createFromFiles({ texturesDirectory + "/skyboxes/sincity/right.tga", texturesDirectory + "/skyboxes/sincity/left.tga",
															 texturesDirectory + "/skyboxes/sincity/top.tga", texturesDirectory + "/skyboxes/sincity/bottom.tga",
															 texturesDirectory + "/skyboxes/sincity/front.tga", texturesDirectory + "/skyboxes/sincity/back.tga" });
		cubemap->generateMipmaps();

		
		std::shared_ptr<Material> skyboxMat = Material::createFromData("skybox_mat",
			{
				shadersDirectory + "skybox.vert",
				shadersDirectory + "skybox.frag"
			},
			{
				{ "cubemap", cubemap }
			}
		);
		

		// create a cube model as skybox
		skybox = Model::createFromFile("skybox", modelsDirectory + "/cube/cube.obj", {
		
		}, false);
		skybox->setMaterial(skyboxMat);
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &destWidth, &destHeight);
		glViewport(0, 0, destWidth, destHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	

		// change depth function so depth test passes when depth values are equal to content of depth buffer
		glDepthFunc(GL_LEQUAL);
		 //render skybox at last
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