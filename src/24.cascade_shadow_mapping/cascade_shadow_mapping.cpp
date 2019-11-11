#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	const uint32_t NUM_CASCADES = 3;

	std::array<std::shared_ptr<Model>, 5> venusShadows;
	std::array<std::shared_ptr<Model>, 5> venuses;
	std::shared_ptr<Model> plane;

	const uint32_t lightMapWidth = 2048;
	const uint32_t lightMapHeight = 2048;
	std::unique_ptr<Framebuffer> lightMapFBO;

	std::array<std::shared_ptr<Texture2D>, 3> lightMaps;

	typedef struct DirectionalLight
	{
		glm::vec3 color;
		glm::vec3 direction;
		float ambientIntensity;
		float diffuseIntensity;
	}dirLight;

	glm::mat4 biasMatrix = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	);

	Example()
	{
		title = "cascade shadow mapping";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/24.cascade_shadow_mapping/";
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
		mMainCamera->setPosition(glm::vec3(0.0f, 10.0f, 15.0f));
		mMainCamera->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		// enable cull face
		glEnable(GL_CULL_FACE);

		lightMapFBO = Framebuffer::create();

		for (std::size_t i = 0; i < lightMaps.size(); i++)
		{
			lightMaps[i] = Texture2D::createFromData(lightMapWidth, lightMapHeight, 1, 1, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, false);
			lightMaps[i]->setMinFilter(GL_LINEAR);
			lightMaps[i]->setMagFilter(GL_LINEAR);
			lightMaps[i]->setCompareMode(GL_NONE);
			lightMaps[i]->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		}

		lightMapFBO->attachDepthRenderTarget(lightMaps[0].get(), 0, 0);

		plane = Model::createFromFile("plane", modelsDirectory + "/rocks_plane/rocks_plane.obj",
			{
				shadersDirectory + "scene.vert",
				shadersDirectory + "scene.frag"
			}
		);
		plane->setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		plane->setScale(glm::vec3(1.0f, 2.0f, 1.0f));

		for (std::size_t i = 0; i < venuses.size(); i++)
		{
			venuses[i] = Model::createFromFile("venus_" + std::to_string(i), modelsDirectory + "/venus/venus.fbx",
				{
					shadersDirectory + "scene.vert",
					shadersDirectory + "scene.frag"
				}
			);
			venuses[i]->setPosition(glm::vec3(0.0f, 0.0f, 15.0f - i * 7.0f));
			venuses[i]->setScale(glm::vec3(0.2f, 0.2f, 0.2f));
		}
		/*
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
		*/
	}

	virtual void render(float deltaTime) override
	{
		/*
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
		*/
		plane->setUniform("lightPos", glm::vec3(0.0f, 10.0f, 0.0f));
		plane->setUniform("viewPos", mMainCamera->getPosition());
		plane->render();

		for (std::size_t i = 0; i < venuses.size(); i++)
		{
			venuses[i]->setUniform("lightPos", glm::vec3(0.0f, 10.0f, 0.0f));
			venuses[i]->setUniform("viewPos", mMainCamera->getPosition());
			venuses[i]->render();
		}
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