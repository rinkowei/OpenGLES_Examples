#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> cube;

	std::unique_ptr<Framebuffer> captureFBO;
	std::unique_ptr<Renderbuffer> captureRBO;

	std::shared_ptr<Texture2D> hdrEnvironmentTexture;
	std::shared_ptr<TextureCube> envCubemap;

	glm::mat4 captureProj;
	std::array<glm::mat4, 6> captureViews;

	const int row = 7;
	const int col = 7;
	std::vector<std::shared_ptr<Model>> spheres;

	struct Light
	{
		glm::vec3 color;
		glm::vec3 position;
	};
	std::array<Light, 6> lights;

	Example()
	{
		title = "spherical harmonics lighting";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/33.spherical_harmonics_lighting/";
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

		// setup camera
		mMainCamera->setPosition(glm::vec3(0.0f, 0.0f, 15.0f));
		mMainCamera->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));

		captureFBO = Framebuffer::create();
		captureRBO = Renderbuffer::create(GL_DEPTH24_STENCIL8, 512, 512);
		captureFBO->addAttachmentRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, captureRBO->getTarget(), captureRBO->getID());

		hdrEnvironmentTexture = Texture2D::createFromFile(texturesDirectory + "/sIBL/Alexs_Apartment.hdr", 1, false);

		envCubemap = TextureCube::createFromData("env_cubemap", 512, 512, 1, GL_RGB16F, GL_RGB, GL_FLOAT, nullptr);
		envCubemap->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		envCubemap->setMagFilter(GL_LINEAR);
		envCubemap->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		captureProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		captureViews =
		{
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
		cube->setUniform("captureProj", captureProj);

		glViewport(0, 0, 512, 512);
		for (unsigned int i = 0; i < 6; i++)
		{
			cube->setUniform("captureView", captureViews[i]);
			captureFBO->addAttachmentTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap->getID(), 0);
			captureFBO->bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cube->render();
		}
		captureFBO->unbind();
		envCubemap->generateMipmaps();

		glViewport(0, 0, mWindowWidth, mWindowHeight);

		std::shared_ptr<Material> pbrMat = Material::createFromData("pbr_mat",
			{
				shadersDirectory + "pbr.vert",
				shadersDirectory + "pbr.frag"
			},
			{

			}
		);

		std::shared_ptr<Model> sphereTemplate = Model::createFromFile("sphere_template", modelsDirectory + "/sphere/sphere.dae",
			{

			},
			false
		);

		sphereTemplate->setMaterial(pbrMat);

		lights[0].position = glm::vec3(-10.0f, -10.0f, 0.0f);
		lights[1].position = glm::vec3(-10.0f, 10.0f, 0.0f);
		lights[2].position = glm::vec3(10.0f, 10.0f, 0.0f);
		lights[3].position = glm::vec3(10.0f, -10.0f, 0.0f);
		lights[4].position = glm::vec3(0.0f, 0.0f, 10.0f);
		lights[5].position = glm::vec3(0.0f, 0.0f, -10.0f);

		for (int x = 0; x < row; x++)
		{
			for (int y = 0; y < col; y++)
			{
				std::shared_ptr<Model> sphere = Model::clone("sphere_" + std::to_string(x * col + y), sphereTemplate.get());

				glm::vec3 pos = glm::vec3(float(y - (row / 2.0f)) * 2.5f, float(x - (col / 2.0f)) * 2.5f, 0.0f);
				sphere->setPosition(pos);
				sphere->setScale(glm::vec3(0.8f));
				sphere->setUniform("albedo", glm::vec3(0.7f, 0.0f, 0.0f));
				sphere->setUniform("roughness", glm::clamp((float)x / (float)(row - 1), 0.05f, 1.0f));
				sphere->setUniform("metallic", glm::clamp((float)y / (float)(col - 1), 0.1f, 1.0f));
				sphere->setUniform("ao", 1.0f);
				sphere->setUniform("exposure", 1.0f);

				for (std::size_t i = 0; i < lights.size(); i++)
				{
					lights[i].color = glm::vec3(100.0f, 100.0f, 100.0f);
					sphere->setUniform("lights[" + std::to_string(i) + "].position", lights[i].position);
					sphere->setUniform("lights[" + std::to_string(i) + "].color", lights[i].color);
				}

				spheres.push_back(sphere);
			}
		}

		std::shared_ptr<Material> backgroundMat = Material::createFromData("background_mat",
			{
				shadersDirectory + "skybox.vert",
				shadersDirectory + "skybox.frag"
			},
			{
				{ "environmentMap", envCubemap }
			}
			);
		cube->setMaterial(backgroundMat);
	}

	virtual void render(float deltaTime) override
	{

		glEnable(GL_CULL_FACE);
		for (std::size_t i = 0; i < spheres.size(); i++)
		{
			spheres[i]->setUniform("viewPos", mMainCamera->getPosition());
			spheres[i]->render();
		}

		glDisable(GL_CULL_FACE);
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