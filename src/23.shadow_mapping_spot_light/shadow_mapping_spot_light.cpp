#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> sampleScene;

	std::shared_ptr<Material> lightPassMat;
	std::shared_ptr<Material> diffuseMat;

	const uint32_t lightMapSize = 2048;
	std::unique_ptr<Framebuffer> lightMapFBO;
	std::shared_ptr<Texture2D> lightMap;

	glm::vec3 lightPos = glm::vec3(0.0f, 5.0f, 0.0f);

	glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);

	glm::mat4 biasMatrix = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	);

	Example()
	{
		title = "shadow mapping spot light";
		settings.vsync = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/23.shadow_mapping_spot_light/";
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

		lightMap = Texture2D::createFromData(lightMapSize, lightMapSize, 1, 1, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, true);
		lightMap->setMinFilter(GL_NEAREST);
		lightMap->setMagFilter(GL_NEAREST);
		lightMap->setWrapping(GL_CLAMP_TO_BORDER_EXT, GL_CLAMP_TO_BORDER_EXT, GL_CLAMP_TO_BORDER_EXT);
		lightMap->setBorderColor(1.0f, 1.0f, 1.0f, 1.0f);

		lightMapFBO = Framebuffer::create();
		lightMapFBO->addAttachmentTexture2D(GL_DEPTH_ATTACHMENT, lightMap->getTarget(), lightMap->getID(), 0);

		lightPassMat = Material::createFromFiles("lightPass_mat",
			{
				shadersDirectory + "light_pass.vert",
				shadersDirectory + "light_pass.frag"
			},
			{

			}
		);

		diffuseMat = Material::createFromData("scene_mat",
			{
				shadersDirectory + "scene.vert",
				shadersDirectory + "scene.frag"
			},
			{
				{ "depthMap", lightMap },
			}
		);

		sampleScene = Model::createFromFile("sampleScene", modelsDirectory + "/teapots-pillars/samplescene.dae", {}, false);
		sampleScene->setScale(glm::vec3(0.05f, 0.05f, 0.05f));
	}

	virtual void render(float deltaTime) override
	{
		lightMapPass();

		glViewport(0, 0, mWindowWidth, mWindowHeight);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		sampleScene->setMaterial(diffuseMat);
		sampleScene->setUniform("lightPos", lightPos);
		sampleScene->setUniform("viewPos", mMainCamera->getPosition());
		sampleScene->setUniform("lightSpaceMatrix", lightSpaceMatrix);
		sampleScene->setUniform("biasMatrix", biasMatrix);
		sampleScene->render();
	}

	virtual void windowResized() override
	{
		ExampleBase::windowResized();
	}

	void lightMapPass()
	{
		// change light position over time
		lightPos = glm::vec3(-2.0f + sin(timePassed) * 2.0f, 5.0f + cos(timePassed) * 1.0f, 2.0f + cos(timePassed) * 1.0f);
		glm::mat4 lightProj = glm::perspective<float>(glm::radians(45.0f), 1.0f, 1.0f, 10.0f);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProj * lightView;

		glViewport(0, 0, lightMapSize, lightMapSize);
		lightMapFBO->bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		sampleScene->setMaterial(lightPassMat);
		sampleScene->setUniform("lightSpaceMatrix", lightSpaceMatrix);
		// fixed peter panning, use cull front face when render scene to depth map
		glCullFace(GL_FRONT);
		sampleScene->render();
		glCullFace(GL_BACK);

		lightMapFBO->unbind();
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