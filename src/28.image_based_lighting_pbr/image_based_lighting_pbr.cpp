#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> cube;

	std::array<std::unique_ptr<Framebuffer>, 6> captureFBOs;
	std::array<std::unique_ptr<Renderbuffer>, 6> captureRBOs;

	Example()
	{
		title = "image based lighting pbr";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/28.image_based_lighting_pbr/";
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
		glDepthFunc(GL_LEQUAL);

		for (std::size_t i = 0; i < captureFBOs.size(); i++)
		{
			captureFBOs[i] = Framebuffer::create();
			captureRBOs[i] = Renderbuffer::create(GL_DEPTH_COMPONENT24, 512, 512);
			captureFBOs[i]->attachRenderBufferTarget(captureRBOs[i].get());
		}

		std::shared_ptr<Texture2D> hdrEnvironmentTexture = Texture2D::createFromFile(texturesDirectory + "/sIBL/TropicalRuins.hdr", 1, false);

		std::shared_ptr<TextureCube> envCubemap = TextureCube::createFromData("env_cubemap", 512, 512, 1, GL_RGB16F, GL_RGB, GL_FLOAT, nullptr);

		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		std::array<glm::mat4, 6> captureViews = {
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		std::shared_ptr<Material> equirectangularToCubemapMat = Material::createFromData("equirectangular_to_cubemap_mat",
			{
				shadersDirectory + "cubemap.vert",
				shadersDirectory + "equirectangular_to_cubemap.frag"
			},
			{
				{ "equirectangularMap", hdrEnvironmentTexture }
			}
		);
		
		cube = Model::createFromFile("cube", modelsDirectory + "/cube/cube.obj", {}, false);
		cube->setMaterial(equirectangularToCubemapMat);
		cube->setUniform("captureProjection", captureProjection);

		glViewport(0, 0, 512, 512);
		for (unsigned int i = 0; i < 6; i++)
		{
			cube->setUniform("captureView", captureViews[i]);
			captureFBOs[0]->attachRenderTarget(0, envCubemap.get(), i, 0);
			captureFBOs[0]->bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cube->render();
			captureFBOs[0]->unbind();
		}

		glViewport(0, 0, mWindowWidth, mWindowHeight);

		std::shared_ptr<Material> backgroundMat = Material::createFromData("background_mat",
			{
				shadersDirectory + "background.vert",
				shadersDirectory + "background.frag"
			},
			{
				{ "environmentMap", envCubemap }
			}
		);
		cube->setMaterial(backgroundMat);
	}

	virtual void render(float deltaTime) override
	{
		cube->render();
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