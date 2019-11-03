#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> roomShadow;
	std::shared_ptr<Model> room;

	const uint16_t lightMapWidth = 2048;
	const uint16_t lightMapHeight = 2048;
	std::array<std::unique_ptr<Framebuffer>, 6> lightMapFBOs;
	std::shared_ptr<TextureCube> lightMap;

	// point light position
	glm::vec3 lightPos = glm::vec3(0.0f, 2.5f, 1.5f);
	// point light matrices
	std::array<glm::mat4, 6> lightMatrices;

	float nearPlane = 1.0f;
	float farPlane = 10.0f;

	Example()
	{
		title = "shadow mapping point light";
		settings.vsync = true;
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
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);

		lightMap = TextureCube::createFromData("light_map", lightMapWidth, lightMapHeight, 1, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		lightMap->setMinFilter(GL_NEAREST);
		lightMap->setMagFilter(GL_NEAREST);
		lightMap->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	
		for (std::size_t i = 0; i < lightMapFBOs.size(); i++)
		{
			lightMapFBOs[i] = Framebuffer::create();
		}

		roomShadow = Model::createFromFile("room_shadow", modelsDirectory + "/van-gogh-room/van-gogh-room.obj",
			{
				shadersDirectory + "depth_map.vert",
				shadersDirectory + "depth_map.frag"
			}
		);
		roomShadow->setUniform("farPlane", farPlane);
	
		room = Model::clone("room", roomShadow.get());
		std::shared_ptr<Material> mat = Material::createFromData("mat",
			{
				shadersDirectory + "room.vert",
				shadersDirectory + "room.frag"
			},
			{
				{ "depthMap", lightMap },
			}
		);

		room->setMaterial(mat);
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &windowWidth, &windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		lightPos.y = cos(glm::radians(timePassed * 45.0f)) + 2.0f;

		glm::mat4 lightProj = glm::perspective(glm::radians(90.0f), (float)lightMapWidth / (float)lightMapHeight, nearPlane, farPlane);
		lightMatrices[0] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		lightMatrices[1] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		lightMatrices[2] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		lightMatrices[3] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		lightMatrices[4] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		lightMatrices[5] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		
		glViewport(0, 0, lightMapWidth, lightMapHeight);
		glCullFace(GL_FRONT);
		for (std::size_t i = 0; i < lightMapFBOs.size(); ++i)
		{
			lightMapFBOs[i]->attachDepthRenderTarget(lightMap.get(), i, 0, 0);
			lightMapFBOs[i]->bind();
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
			roomShadow->setUniform("farPlane", farPlane);
			roomShadow->setUniform("lightPos", lightPos);
			roomShadow->setUniform("lightMatrix", lightMatrices[i]);
			roomShadow->render();
			lightMapFBOs[i]->unbind();
		}
		glCullFace(GL_BACK);
		
		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		room->setUniform("farPlane", farPlane);
		room->setUniform("lightPos", lightPos);
		room->setUniform("viewPos", mMainCamera->getPosition());
		room->render();
		
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