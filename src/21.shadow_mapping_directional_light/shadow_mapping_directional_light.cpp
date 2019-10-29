#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> playground;

	const uint16_t lightMapWidth = 2048;
	const uint16_t lightMapHeight = 2048;
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
		settings.validation = true;
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

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		// enable cull face
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);
		glCullFace(GL_BACK);

		
		std::shared_ptr<Texture2D> lightMap = Texture2D::createFromData(lightMapWidth, lightMapHeight, 1, 1, 1, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);
		lightMap->setMinFilter(GL_NEAREST);
		lightMap->setMagFilter(GL_NEAREST);
		lightMap->setWrapping(GL_CLAMP_TO_BORDER_NV, GL_CLAMP_TO_BORDER_NV, GL_CLAMP_TO_BORDER_NV);
		lightMap->setBorderColor(1.0f, 1.0f, 1.0f, 1.0f);

		lightMapFBO = Framebuffer::create();
		//lightMapFBO->attachDepthRenderTarget(lightMap.get(), 0, 0);
		lightMapFBO->attachDepthStencilTarget(lightMapWidth, lightMapHeight);
		
		playground = Model::createFromFile("playground_shadow", modelsDirectory + "/playground/Playground.obj",
			{
				shadersDirectory + "depth_map.vert",
				shadersDirectory + "depth_map.frag"
			}
		);
		/*
		std::unordered_map<Material::ShaderType, std::string> depthShaderPaths =
		{
			{ Material::ShaderType::VERTEX, shadersDirectory + "depth_map.vert" },
			{ Material::ShaderType::FRAGMENT, shadersDirectory + "depth_map.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> depthTexturePaths =
		{
		
		};

		depthMapMat = std::make_shared<Material>(depthShaderPaths, depthTexturePaths);

		std::unordered_map<Material::ShaderType, std::string> groundShaderPaths =
		{
			{ Material::ShaderType::VERTEX, shadersDirectory + "playground.vert" },
			{ Material::ShaderType::FRAGMENT, shadersDirectory + "playground.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> groundTexturePaths =
		{
			
		};

		std::shared_ptr<Material> groundMat = std::make_shared<Material>(groundShaderPaths, groundTexturePaths);

		playground = Model::createWithFile(modelsDirectory + "/playground/Playground.obj", groundMat);
		playground->setMatrix4x4("biasMatrix", biasMatrix);
		playground->setInteger("depthMap", 0);

		*/
		/*
		GLuint depthMapFBO;
		GLuint depthTexture;
		glGenFramebuffers(1, &depthMapFBO);
		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, lightMapWidth, lightMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_NV);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_NV);
		std::array<float, 4> borderColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR_NV, borderColor.data());
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapFBO, 0);
		glDrawBuffers(0, GL_NONE);
		glReadBuffer(GL_NONE);

		GLenum status = GLES_CHECK_ERROR(glCheckFramebufferStatus(GL_FRAMEBUFFER));

		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::string error = "Framebuffer incomplete : ";

			switch (status)
			{
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			{
				error += "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
				break;
			}
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			{
				error += "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
				break;
			}
			case GL_FRAMEBUFFER_UNSUPPORTED:
			{
				error += "GL_FRAMEBUFFER_UNSUPPORTED";
				break;
			}
			default:
				break;
			}

			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, error.c_str());
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		*/
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &destWidth, &destHeight);
		glViewport(0, 0, destWidth, destHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		// change light position over time
		lightPos = glm::vec3(sin(SDL_GetTicks()) * 2.0f, 5.0f + cos(SDL_GetTicks()) * 1.0f, cos(SDL_GetTicks()) * 1.0f);
		glm::mat4 lightProj = glm::ortho<float>(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 10.0f);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProj * lightView;

		playground->setUniform("lightSpaceMatrix", lightSpaceMatrix);
		playground->render();
		/*
		World::getWorld()->enableGlobalMaterial(depthMapMat);

		// change light position over time
		lightPos = glm::vec3(sin(glfwGetTime()) * 2.0f, 5.0f + cos(glfwGetTime()) * 1.0f, cos(glfwGetTime()) * 1.0f);
		glm::mat4 lightProj = glm::ortho<float>(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 10.0f);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProj * lightView;
		depthMapMat->setMatrix4x4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, depthMapWidth, depthMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// fixed peter panning, use cull front face when render scene to depth map
		glCullFace(GL_FRONT);
		depthMapMat->setMatrix4x4("model", playground->getModelMatrix());
		playground->render(deltaTime);
		glCullFace(GL_BACK);

		World::getWorld()->disableGlobalMaterial();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexture);

		playground->setVector3("lightPos", lightPos);
		playground->setVector3("viewPos", camera->getPosition());
		playground->setMatrix4x4("lightSpaceMatrix", lightSpaceMatrix);
		playground->render(deltaTime);
		*/
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