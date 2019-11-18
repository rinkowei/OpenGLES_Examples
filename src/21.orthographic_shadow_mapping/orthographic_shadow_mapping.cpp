#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> playground;

	std::shared_ptr<Material> lightPassMat;
	std::shared_ptr<Material> diffuseMat;

	const uint32_t lightMapSize = 2048;
	std::unique_ptr<Framebuffer> lightMapFBO;
	std::shared_ptr<Texture2D> lightMap;

	glm::vec3 lightDir = glm::vec3(1.0f, -1.0f, 0.0f);
	glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);

	glm::mat4 biasMatrix = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	);

	Example()
	{
		title = "orthographic shadow mapping";
		settings.vsync = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/21.orthographic_shadow_mapping/";
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
		
		lightMap = Texture2D::createFromData(lightMapSize, lightMapSize, 1, 1, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, true);
		lightMap->setMinFilter(GL_NEAREST);
		lightMap->setMagFilter(GL_NEAREST);
		lightMap->setCompareMode(GL_COMPARE_REF_TO_TEXTURE);
		lightMap->setCompareFunc(GL_LEQUAL);
		lightMap->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		
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

		diffuseMat = Material::createFromData("playground_mat",
			{
				shadersDirectory + "playground.vert",
				shadersDirectory + "playground.frag"
			},
			{
				{ "depthMap", lightMap },
			}
		);

		playground = Model::createFromFile("playground", modelsDirectory + "/playground/Playground.obj", {}, false);
	}

	virtual void render(float deltaTime) override
	{
		lightMapPass();

		glViewport(0, 0, mWindowWidth, mWindowHeight);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		playground->setMaterial(diffuseMat);
		playground->setUniform("lightDir", lightDir);
		playground->setUniform("viewPos", mMainCamera->getPosition());
		playground->setUniform("lightSpaceMatrix", lightSpaceMatrix);
		playground->setUniform("biasMatrix", biasMatrix);
		playground->render();
	}

	virtual void windowResized() override
	{
		ExampleBase::windowResized();
	}

	void lightMapPass()
	{
		// change light position over time
		lightDir = glm::vec3(sin(timePassed) * 1.0f, -1.0f, cos(timePassed) * 1.0f);
		glm::mat4 lightProj = glm::ortho<float>(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 10.0f);
		glm::mat4 lightView = glm::lookAtLH<float>(lightDir, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		lightSpaceMatrix = lightProj * lightView;

		glViewport(0, 0, lightMapSize, lightMapSize);
		lightMapFBO->bind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		playground->setMaterial(lightPassMat);
		playground->setUniform("lightSpaceMatrix", lightSpaceMatrix);

		glCullFace(GL_FRONT);
		playground->render();
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