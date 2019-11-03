#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> sampleSceneShadow;
	std::shared_ptr<Model> sampleScene;

	const uint16_t lightMapWidth = 2048;
	const uint16_t lightMapHeight = 2048;
	std::unique_ptr<Framebuffer> lightMapFBO;

	glm::vec3 lightPos = glm::vec3(0.0f, 5.0f, 0.0f);

	glm::mat4 biasMatrix = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	);

	Example()
	{
		title = "variance shadow mapping";
		settings.vsync = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/25.variance_shadow_mapping/";
	}

	~Example()
	{

	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		mMainCamera->setPosition(glm::vec3(0.0f, 2.0f, 3.0f));
		mMainCamera->setRotation(glm::vec3(30.0f, 0.0f, 0.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		// enable cull face
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);

		std::shared_ptr<Texture2D> lightMap = Texture2D::createFromData(lightMapWidth, lightMapHeight, 1, 1, 1, GL_RGBA32F, GL_RGBA, GL_FLOAT);
		lightMap->setMinFilter(GL_NEAREST);
		lightMap->setMagFilter(GL_NEAREST);
		lightMap->setWrapping(GL_CLAMP_TO_BORDER_EXT, GL_CLAMP_TO_BORDER_EXT, GL_CLAMP_TO_BORDER_EXT);
		lightMap->setBorderColor(1.0f, 1.0f, 1.0f, 1.0f);

		lightMapFBO = Framebuffer::create();
		lightMapFBO->attachRenderTarget(0, lightMap.get(), 0, 0);

		sampleSceneShadow = Model::createFromFile("sampleScene_shadow", modelsDirectory + "/teapots-pillars/samplescene.dae",
			{
				shadersDirectory + "depth_map.vert",
				shadersDirectory + "depth_map.frag"
			}
		);
		sampleSceneShadow->setScale(glm::vec3(0.05f, 0.05f, 0.05f));
		
		sampleScene = Model::clone("sampleScene", sampleSceneShadow.get());
		std::shared_ptr<Material> mat = Material::createFromData("mat",
			{
				shadersDirectory + "scene.vert",
				shadersDirectory + "scene.frag"
			},
			{
				{ "depthMap", lightMap },
			}
			);
		sampleScene->setMaterial(mat);
		sampleScene->setUniform("biasMatrix", biasMatrix);
		sampleScene->setScale(sampleSceneShadow->getScaling());
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &windowWidth, &windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// change light position over time
		lightPos = glm::vec3(-2.0f + sin(timePassed) * 2.0f, 5.0f + cos(timePassed) * 1.0f, 2.0f + cos(timePassed) * 1.0f);
		glm::mat4 lightProj = glm::perspective<float>(glm::radians(45.0f), 1.0f, 1.0f, 10.0f);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 lightSpaceMatrix = lightProj * lightView;
		
		glViewport(0, 0, lightMapWidth, lightMapHeight);
		lightMapFBO->bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		sampleSceneShadow->setUniform("lightSpaceMatrix", lightSpaceMatrix);

		glCullFace(GL_FRONT);
		sampleSceneShadow->render();
		glCullFace(GL_BACK);

		lightMapFBO->unbind();
		glViewport(0, 0, windowWidth, windowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		sampleScene->setUniform("lightPos", lightPos);
		sampleScene->setUniform("viewPos", mMainCamera->getPosition());
		sampleScene->setUniform("lightSpaceMatrix", lightSpaceMatrix);
		sampleScene->render();
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