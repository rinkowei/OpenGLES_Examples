#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> roomShadow;
	std::shared_ptr<Model> room;

	const uint16_t lightMapWidth = 1024;
	const uint16_t lightMapHeight = 1024;
	std::unique_ptr<Framebuffer> lightMapFBO;
	std::shared_ptr<TextureCube> lightMap;

	GLuint depthMapFBO;
	GLuint depthCubeMap;

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
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);

		lightMap = TextureCube::createFromData("light_map", lightMapWidth, lightMapHeight, 1, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		lightMap->setMinFilter(GL_NEAREST);
		lightMap->setMagFilter(GL_NEAREST);
		lightMap->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		
		lightMapFBO = Framebuffer::create();

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
				//{ "depthMap", lightMap },
			}
		);
		room->setMaterial(mat);

		// configure depth map FBO

		glGenFramebuffers(1, &depthMapFBO);
		glGenTextures(1, &depthCubeMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
		for (unsigned int i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F, lightMapWidth, lightMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glDrawBuffers(0, GL_NONE);
		glReadBuffer(GL_NONE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &destWidth, &destHeight);
		glViewport(0, 0, destWidth, destHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glm::mat4 lightProj = glm::perspective(glm::radians(90.0f), (float)lightMapWidth / (float)lightMapHeight, nearPlane, farPlane);
		lightMatrices[0] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		lightMatrices[1] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		lightMatrices[2] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		lightMatrices[3] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		lightMatrices[4] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		lightMatrices[5] = lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		
		glViewport(0, 0, lightMapWidth, lightMapHeight);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		for (std::size_t i = 0; i < lightMatrices.size(); ++i)
		{
			GLuint face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, face, depthCubeMap, 0);
			roomShadow->setUniform("farPlane", farPlane);
			roomShadow->setUniform("lightPos", lightPos);
			roomShadow->setUniform("lightMatrix", lightMatrices[i]);
			roomShadow->render();
		}
		glCullFace(GL_BACK);
		
		glViewport(0, 0, defaultWindowWidth, defaultWindowHeight);
		lightMapFBO->unbind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		room->setUniform("farPlane", farPlane);
		room->setUniform("lightPos", lightPos);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
		room->setUniform("depthMap", 0);
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