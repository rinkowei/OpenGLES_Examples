#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> room;

	std::shared_ptr<Material> lightPassMat;
	std::shared_ptr<Material> diffuseMat;

	const uint32_t lightMapSize = 2048;
	std::unique_ptr<Framebuffer> lightMapFBO;
	std::shared_ptr<TextureCube> lightMap;

	// point light position
	glm::vec3 lightPos = glm::vec3(0.0f, 2.5f, 1.5f);
	// point light matrices
	std::array<glm::mat4, 6> lightSpaceMatrices;

	const float nearPlane = 1.0f;
	const float farPlane = 10.0f;

	Example()
	{
		title = "shadow mapping point light";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/22.shadow_mapping_point_light/";
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
		mMainCamera->setRotation(glm::vec3(45.0f, 0.0f, 0.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		// enable cull face
		glEnable(GL_CULL_FACE);

		lightMap = TextureCube::createFromData("light_map", lightMapSize, lightMapSize, 1, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		lightMap->setMinFilter(GL_NEAREST);
		lightMap->setMagFilter(GL_NEAREST);
		lightMap->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	
		lightMapFBO = Framebuffer::create();

		lightPassMat = Material::createFromFiles("lightPass_mat",
			{
				shadersDirectory + "light_pass.vert",
				shadersDirectory + "light_pass.frag"
			},
			{

			}
		);

		diffuseMat = Material::createFromData("room_mat",
			{
				shadersDirectory + "room.vert",
				shadersDirectory + "room.frag"
			},
			{
				{ "depthMap", lightMap },
			}
		);

		room = Model::createFromFile("room", modelsDirectory + "/van-gogh-room/van-gogh-room.obj", {}, false);
	}

	virtual void render(float deltaTime) override
	{
		lightMapPass();

		glViewport(0, 0, mWindowWidth, mWindowHeight);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		room->setMaterial(diffuseMat);
		room->setUniform("farPlane", farPlane);
		room->setUniform("lightPos", lightPos);
		room->setUniform("viewPos", mMainCamera->getPosition());
		room->render();
	}

	virtual void windowResized() override
	{
		ExampleBase::windowResized();
	}

	void lightMapPass()
	{
		lightPos.y = cos(glm::radians(timePassed * 45.0f)) + 2.0f;

		glm::mat4 lightProj = glm::perspective(glm::radians(90.0f), (float)lightMapSize / (float)lightMapSize, nearPlane, farPlane);
		lightSpaceMatrices[0] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		lightSpaceMatrices[1] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		lightSpaceMatrices[2] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		lightSpaceMatrices[3] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		lightSpaceMatrices[4] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		lightSpaceMatrices[5] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

		glViewport(0, 0, lightMapSize, lightMapSize);
		glCullFace(GL_FRONT);

		room->setMaterial(lightPassMat);
		for (std::size_t i = 0; i < 6; ++i)
		{
			lightMapFBO->attachDepthRenderTarget(lightMap.get(), i, 0, 0);
			lightMapFBO->bind();
			glClear(GL_DEPTH_BUFFER_BIT);

			room->setUniform("farPlane", farPlane);
			room->setUniform("lightPos", lightPos);
			room->setUniform("lightSpaceMatrix", lightSpaceMatrices[i]);
			room->render();
		}
		lightMapFBO->unbind();
		glCullFace(GL_BACK);
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