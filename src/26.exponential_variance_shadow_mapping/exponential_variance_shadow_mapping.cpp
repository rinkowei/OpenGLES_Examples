#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> sampleScene;
	std::shared_ptr<Model> blurQuad;

	std::shared_ptr<Material> sceneMat;
	std::shared_ptr<Material> lightPassMat;
	std::shared_ptr<Material> blurMat;

	std::shared_ptr<Texture2D> lightMap;
	std::shared_ptr<Texture2D> gaussianBlurrdedLightMap;

	std::unique_ptr<Framebuffer> blurFBO;
	std::unique_ptr<Framebuffer> lightMapFBO;
	const uint16_t lightMapSize = 2048;

	glm::vec3 lightPos = glm::vec3(0.0f, 5.0f, 0.0f);

	glm::mat4 biasMatrix = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	);

	glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);

	Example()
	{
		title = "exponential variance shadow mapping";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/26.exponential_variance_shadow_mapping/";
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

		lightMap = Texture2D::createFromData(lightMapSize, lightMapSize, 1, 1, GL_RGBA32F, GL_RGBA, GL_FLOAT, true);
		lightMap->setMinFilter(GL_LINEAR);
		lightMap->setMagFilter(GL_LINEAR);
		lightMap->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		lightMapFBO = Framebuffer::create();
		lightMapFBO->addAttachmentTexture2D(GL_COLOR_ATTACHMENT0, lightMap->getTarget(), lightMap->getID(), 0);

		gaussianBlurrdedLightMap = Texture2D::createFromData(lightMapSize, lightMapSize, 1, 1, GL_RGBA32F, GL_RGBA, GL_FLOAT, true);
		gaussianBlurrdedLightMap->setMinFilter(GL_NEAREST);
		gaussianBlurrdedLightMap->setMagFilter(GL_NEAREST);
		gaussianBlurrdedLightMap->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		blurFBO = Framebuffer::create();
		blurFBO->addAttachmentTexture2D(GL_COLOR_ATTACHMENT0, gaussianBlurrdedLightMap->getTarget(), gaussianBlurrdedLightMap->getID(), 0);

		sceneMat = Material::createFromData("scene_mat",
			{
				shadersDirectory + "scene.vert",
				shadersDirectory + "scene.frag"
			},
			{
				{ "depthMap", gaussianBlurrdedLightMap },
			}
		);

		lightPassMat = Material::createFromFiles("light_pass_mat",
			{
				shadersDirectory + "light_pass.vert",
				shadersDirectory + "light_pass.frag"
			},
			{

			}
		);

		blurMat = Material::createFromData("blur_mat",
			{
				shadersDirectory + "blur.vert",
				shadersDirectory + "blur.frag"
			},
			{
				{ "inputImage", lightMap }
			}
		);

		sampleScene = Model::createFromFile("sample_scene", modelsDirectory + "/teapots-pillars/samplescene.dae", {}, false);
		sampleScene->setScale(glm::vec3(0.05f, 0.05f, 0.05f));

		blurQuad = Model::createFromFile("blur_quad", modelsDirectory + "/quadrangle/quadrangle.obj", {}, false);
		blurQuad->setMaterial(blurMat);
	}

	virtual void render(float deltaTime) override
	{
		lightMapPass();

		lightMapGaussianBlur();

		glViewport(0, 0, mWindowWidth, mWindowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		sampleScene->setMaterial(sceneMat);
		sampleScene->setUniform("biasMatrix", biasMatrix);
		sampleScene->setUniform("lightPos", lightPos);
		sampleScene->setUniform("viewPos", mMainCamera->getPosition());
		sampleScene->setUniform("lightSpaceMatrix", lightSpaceMatrix);
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

		lightMapFBO->bind();
		glViewport(0, 0, lightMapSize, lightMapSize);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		sampleScene->setMaterial(lightPassMat);
		sampleScene->setUniform("lightSpaceMatrix", lightSpaceMatrix);

		glCullFace(GL_FRONT);
		sampleScene->render();
		glCullFace(GL_BACK);

		lightMapFBO->unbind();
	}

	void lightMapGaussianBlur()
	{
		blurFBO->bind();
		glViewport(0, 0, lightMapSize, lightMapSize);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		blurQuad->render();

		blurFBO->unbind();
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