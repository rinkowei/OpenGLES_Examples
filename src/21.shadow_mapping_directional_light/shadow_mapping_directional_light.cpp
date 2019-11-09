#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> playgroundShadow;
	std::shared_ptr<Model> playground;

	const uint32_t lightMapWidth = 2048;
	const uint32_t lightMapHeight = 2048;
	std::unique_ptr<Framebuffer> lightMapFBO;

	glm::vec3 lightPos = glm::vec3(-2.0f, 5.0f, 0.0f);

	glm::mat4 biasMatrix = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	);

	Example()
	{
		title = "shadow mapping directional light";
		settings.vsync = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/21.shadow_mapping_directional_light/";
		texturesDirectory = getResourcesPath(ResourceType::Texture);
	}
	~Example()
	{

	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();
		
		// setup camera
		mMainCamera->setPosition(glm::vec3(-2.0f, 4.0f, 5.0f));
		mMainCamera->setRotation(glm::vec3(45.0f, 0.0f, 0.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		// enable cull face
		glEnable(GL_CULL_FACE);
		
		std::shared_ptr<Texture2D> lightMap = Texture2D::createFromData(lightMapWidth, lightMapHeight, -1, 1, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, false);
		lightMap->setMinFilter(GL_NEAREST);
		lightMap->setMagFilter(GL_NEAREST);
		lightMap->setWrapping(GL_CLAMP_TO_BORDER_EXT, GL_CLAMP_TO_BORDER_EXT, GL_CLAMP_TO_BORDER_EXT);
		lightMap->setBorderColor(1.0f, 1.0f, 1.0f, 1.0f);
		
		lightMapFBO = Framebuffer::create();
		lightMapFBO->attachDepthRenderTarget(lightMap.get(), 0, 0);
		
		playgroundShadow = Model::createFromFile("playground_shadow", modelsDirectory + "/playground/Playground.obj",
			{
				shadersDirectory + "depth_map.vert",
				shadersDirectory + "depth_map.frag"
			}
		);

		playground = Model::clone("playground", playgroundShadow.get());
		std::shared_ptr<Material> mat = Material::createFromData("mat",
			{
				shadersDirectory + "playground.vert",
				shadersDirectory + "playground.frag"
			},
			{
				{ "depthMap", lightMap },
			}
		);
		playground->setMaterial(mat);
		playground->setUniform("biasMatrix", biasMatrix);
		
	}

	virtual void render(float deltaTime) override
	{
		// change light position over time
		lightPos = glm::vec3(sin(timePassed) * 2.0f, 5.0f + cos(timePassed) * 1.0f, cos(timePassed) * 1.0f);
		glm::mat4 lightProj = glm::ortho<float>(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 10.0f);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProj * lightView;

		glViewport(0, 0, lightMapWidth, lightMapHeight);
		lightMapFBO->bind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		playgroundShadow->setUniform("lightSpaceMatrix", lightSpaceMatrix);

		glCullFace(GL_FRONT);
		playgroundShadow->render();
		glCullFace(GL_BACK);
		
		lightMapFBO->unbind();
		glViewport(0, 0, mWindowWidth, mWindowHeight);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		playground->setUniform("lightPos", lightPos);
		playground->setUniform("viewPos", mMainCamera->getPosition());
		playground->setUniform("lightSpaceMatrix", lightSpaceMatrix);
		playground->render();
		
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