#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> cube;
	std::shared_ptr<Model> quad;
	std::shared_ptr<Model> drakefirePistol;

	std::unique_ptr<Framebuffer> captureFBO;
	std::unique_ptr<Renderbuffer> captureRBO;

	std::shared_ptr<Texture2D> hdrEnvironmentTexture;
	std::shared_ptr<TextureCube> envCubemap;
	std::shared_ptr<TextureCube> irradianceCubemap;

	std::shared_ptr<TextureCube> prefilterCubemap;
	std::shared_ptr<Texture2D> brdfLUT;

	glm::mat4 captureProj;
	std::array<glm::mat4, 6> captureViews;

	struct Light
	{
		glm::vec3 color;
		glm::vec3 position;
	};
	std::array<Light, 4> lights;

	const float exposure = 2.0f;

	Example()
	{
		title = "image based lighting pbr textured";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/31.image_based_lighting_pbr_textured/";
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
		mMainCamera->setPosition(glm::vec3(-10.0f, 0.0f, 30.0f));
		mMainCamera->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
		mMainCamera->setMoveSensitivity(50.0f);

		captureFBO = Framebuffer::create();
		captureRBO = Renderbuffer::create(GL_DEPTH24_STENCIL8, 512, 512);
		captureFBO->addAttachmentRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, captureRBO->getTarget(), captureRBO->getID());

		hdrEnvironmentTexture = Texture2D::createFromFile(texturesDirectory + "/sIBL/Alexs_Apartment.hdr", 1, false);

		envCubemap = TextureCube::createFromData("env_cubemap", 512, 512, 1, GL_RGB16F, GL_RGB, GL_FLOAT, nullptr);
		envCubemap->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		envCubemap->setMagFilter(GL_LINEAR);
		envCubemap->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		irradianceCubemap = TextureCube::createFromData("irradiance_cubemap", 32, 32, 1, GL_RGB16F, GL_RGB, GL_FLOAT, nullptr);
		irradianceCubemap->setMinFilter(GL_LINEAR);
		irradianceCubemap->setMagFilter(GL_LINEAR);
		irradianceCubemap->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		
		prefilterCubemap = TextureCube::createFromData("prefilter_cubemap", 128, 128, 1, GL_RGB16F, GL_RGB, GL_FLOAT, nullptr);
		prefilterCubemap->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		prefilterCubemap->setMagFilter(GL_LINEAR);
		prefilterCubemap->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		prefilterCubemap->generateMipmaps();

		brdfLUT = Texture2D::createFromData(512, 512, 1, 1, GL_RG16F, GL_RG, GL_FLOAT, false);
		brdfLUT->setMinFilter(GL_LINEAR);
		brdfLUT->setMagFilter(GL_LINEAR);
		brdfLUT->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		
		captureProj= glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
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

		std::shared_ptr<Material> irradianceMat = Material::createFromData("irradiance_mat",
			{
				shadersDirectory + "cubemap.vert",
				shadersDirectory + "irradiance_convolution.frag"
			},
			{
				{ "environmentMap", envCubemap }
			}
		);

		captureFBO->bind();
		captureRBO->resize(32, 32);
		glViewport(0, 0, 32, 32);

		cube->setMaterial(irradianceMat);
		cube->setUniform("captureProj", captureProj);
		for (unsigned int i = 0; i < 6; i++)
		{
			cube->setUniform("captureView", captureViews[i]);
			captureFBO->addAttachmentTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceCubemap->getID(), 0);
			captureFBO->bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cube->render();
		}
		captureFBO->unbind();

		std::shared_ptr<Material> prefilterMat = Material::createFromData("prefilter_mat",
			{
				shadersDirectory + "cubemap.vert",
				shadersDirectory + "prefilter.frag"
			},
			{
				{ "environmentMap", envCubemap }
			}
		);
		cube->setMaterial(prefilterMat);
		cube->setUniform("captureProj", captureProj);

		captureFBO->bind();
		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			unsigned int mipWidth = 128 * std::pow(0.5, mip);
			unsigned int mipHeight = 128 * std::pow(0.5, mip);
			captureRBO->resize(mipWidth, mipHeight);

			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			cube->setUniform("roughness", roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				cube->setUniform("captureView", captureViews[i]);
				captureFBO->addAttachmentTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterCubemap->getID(), mip);
				captureFBO->bind();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				cube->render();
			}
		}
		captureFBO->unbind();

		std::shared_ptr<Material> brdfMat = Material::createFromFiles("brdf_mat",
			{
				shadersDirectory + "brdf.vert",
				shadersDirectory + "brdf.frag"
			},
			{
				
			}
		);

		quad = Model::createFromFile("quad", modelsDirectory + "/quadrangle/quadrangle.obj", {}, false);
		quad->setMaterial(brdfMat);

		captureFBO->addAttachmentTexture2D(GL_COLOR_ATTACHMENT0, brdfLUT->getTarget(), brdfLUT->getID(), 0);
		captureFBO->bind();
		captureRBO->resize(512, 512);
		glViewport(0, 0, 512, 512);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		quad->render();
		captureFBO->unbind();
	
		glViewport(0, 0, mWindowWidth, mWindowHeight);

		std::shared_ptr<Material> pbrMat = Material::createFromFiles("pbr_mat",
			{
				shadersDirectory + "pbr.vert",
				shadersDirectory + "pbr.frag"
			},
			{
				{ "albedoMap", modelsDirectory + "/drakefire_pistol/albedo.jpg" },
				{ "metallicMap", modelsDirectory + "/drakefire_pistol/metallic.jpg" },
				{ "normalMap", modelsDirectory + "/drakefire_pistol/normal.jpg" },
				{ "roughnessMap", modelsDirectory + "/drakefire_pistol/roughness.jpg" },
				{ "aoMap", modelsDirectory + "/drakefire_pistol/ao.jpg" },
			}
		);

		drakefirePistol = Model::createFromFile("drakefire_pistol", modelsDirectory + "/drakefire_pistol/drakefire_pistol.obj", {}, false);
		drakefirePistol->setMaterial(pbrMat);
		drakefirePistol->setTexture("irradianceMap", irradianceCubemap);
		drakefirePistol->setTexture("prefilterMap", prefilterCubemap);
		drakefirePistol->setTexture("brdfLUT", brdfLUT);

		lights[0].position = glm::vec3(-15.0f, -10.0f, -15.0f);
		lights[1].position = glm::vec3(-15.0f, -10.0f, 15.0f);
		lights[2].position = glm::vec3(15.0f, -10.0f, 15.0f);
		lights[3].position = glm::vec3(15.0f, -10.0f, -15.0f);

		drakefirePistol->setRotation(glm::vec3(0.0f, -90.0f, 0.0f));
		drakefirePistol->setScale(glm::vec3(10.0f));
		drakefirePistol->setUniform("exposure", exposure);

		for (std::size_t i = 0; i < lights.size(); i++)
		{
			lights[i].color = glm::vec3(100.0f, 100.0f, 100.0f);
			drakefirePistol->setUniform("lights[" + std::to_string(i) + "].position", lights[i].position);
			drakefirePistol->setUniform("lights[" + std::to_string(i) + "].color", lights[i].color);
		}
		
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
		cube->setUniform("exposure", exposure);
	}

	virtual void render(float deltaTime) override
	{
		
		glEnable(GL_CULL_FACE);
		drakefirePistol->setUniform("viewPos", mMainCamera->getPosition());
		drakefirePistol->render();

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