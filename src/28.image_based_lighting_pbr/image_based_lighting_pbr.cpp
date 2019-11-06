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

	const int row = 7;
	const int col = 7;
	std::vector<std::shared_ptr<Model>> spheres;

	struct Light
	{
		glm::vec3 color;
		glm::vec3 position;
	};
	std::array<Light, 4> lights;

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

		// setup camera
		mMainCamera->setPosition(glm::vec3(0.0f, 0.0f, 15.0f));
		mMainCamera->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));

		captureFBO = Framebuffer::create();
		captureRBO = Renderbuffer::create(GL_DEPTH_COMPONENT24, 512, 512);
		captureFBO->attachRenderBufferTarget(captureRBO.get());

		std::shared_ptr<Texture2D> hdrEnvironmentTexture = Texture2D::createFromFile(texturesDirectory + "/sIBL/newport_loft.hdr", 1, false);

		std::shared_ptr<TextureCube> envCubemap = TextureCube::createFromData("env_cubemap", 512, 512, 1, GL_RGB16F, GL_RGB, GL_FLOAT, nullptr);
		envCubemap->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		envCubemap->setMagFilter(GL_LINEAR);

		std::shared_ptr<TextureCube> irradianceCubemap = TextureCube::createFromData("irradiance_cubemap", 32, 32, 1, GL_RGB16F, GL_RGB, GL_FLOAT, nullptr);

		std::shared_ptr<TextureCube> prefilterMap = TextureCube::createFromData("prefilter_cubemap", 128, 128, 1, GL_RGB16F, GL_RGB, GL_FLOAT, nullptr);
		prefilterMap->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		prefilterMap->setMagFilter(GL_LINEAR);
		prefilterMap->generateMipmaps();

		std::shared_ptr<Texture2D> brdfLUTTexture = Texture2D::createFromData(512, 512, 1, 1, 1, GL_RG16F, GL_RG, GL_FLOAT);
		brdfLUTTexture->setMinFilter(GL_LINEAR);
		brdfLUTTexture->setMagFilter(GL_LINEAR);

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
			captureFBO->attachRenderTarget(0, envCubemap.get(), i, 0);
			captureFBO->bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cube->render();
		}
		envCubemap->generateMipmaps();

		captureRBO->resize(32, 32);

		std::shared_ptr<Material> irradianceMat = Material::createFromData("irradiance_mat",
			{
				shadersDirectory + "cubemap.vert",
				shadersDirectory + "irradiance_convolution.frag"
			},
			{
				{ "environmentMap", envCubemap }
			}
		);
		cube->setMaterial(irradianceMat);
		cube->setUniform("captureProjection", captureProjection);

		glViewport(0, 0, 32, 32);
		for (unsigned int i = 0; i < 6; i++)
		{
			cube->setUniform("captureView", captureViews[i]);
			captureFBO->attachRenderTarget(0, irradianceCubemap.get(), i, 0);
			captureFBO->bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cube->render();
		}


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
		cube->setUniform("captureProjection", captureProjection);

	    uint32_t maxMipLevels = 5;
		for (uint32_t mip = 0; mip < maxMipLevels; ++mip)
		{
			uint32_t mipWidth = 128 * std::pow(0.5, mip);
			uint32_t mipHeight = 128 * std::pow(0.5, mip);
			captureRBO->resize(mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			cube->setUniform("roughness", roughness);
			for (unsigned int i = 0; i < 6; i++)
			{
				cube->setUniform("captureView", captureViews[i]);
				captureFBO->attachRenderTarget(0, prefilterMap.get(), i, mip);
				captureFBO->bind();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				cube->render();
			}
		}
		
		captureRBO->resize(512, 512);
		captureFBO->attachRenderTarget(0, brdfLUTTexture.get(), 0, 0);
		captureFBO->bind();
		glViewport(0, 0, 512, 512);

		std::shared_ptr<Material> brdfMat = Material::createFromFiles("brdf_mat",
			{
				shadersDirectory + "brdf.vert",
				shadersDirectory + "brdf.frag"
			},
			{
			
			}
		);

		std::vector<float> vertexAttribs = {
			// positions          // texture coordinates
			0.5f,  0.5f, 0.0f,    1.0f, 1.0f,
			0.5f, -0.5f, 0.0f,    1.0f, 0.0f,
		   -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,
		   -0.5f,  0.5f, 0.0f,    0.0f, 1.0f
		};

		std::vector<uint32_t> indices = {
			0, 1, 3,
			1, 2, 3
		};

		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexAttribs.size() / 5); i++)
		{
			Vertex vertex;
			vertex.vPosition = glm::vec3(vertexAttribs[i * 5], vertexAttribs[i * 5 + 1], vertexAttribs[i * 5 + 2]);
			vertex.vTexcoord = glm::vec2(vertexAttribs[i * 5 + 3], vertexAttribs[i * 5 + 4]);
			vertices.push_back(vertex);
		}

		std::shared_ptr<Mesh> quad = Mesh::createWithData("quad", vertices, indices);
		quad->setDrawType(Mesh::DrawType::ELEMENTS);
		quad->setMaterial(brdfMat);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		quad->render();
		captureFBO->unbind();

		glViewport(0, 0, mWindowWidth, mWindowHeight);

		std::shared_ptr<Material> pbrMat = Material::createFromData("pbr_mat",
			{
				shadersDirectory + "pbr.vert",
				shadersDirectory + "pbr.frag"
			},
			{
				{ "irradianceMap", irradianceCubemap },
				{ "prefilterMap", prefilterMap },
				{ "brdfLUT", brdfLUTTexture }
			}
		);
		std::shared_ptr<Model> sphereTemplate = Model::createFromFile("sphere_template", modelsDirectory + "/sphere/sphere.obj",
			{

			},
			false
		);
		sphereTemplate->setMaterial(pbrMat);

		lights[0].position = glm::vec3(-10.0f, 10.0f, 10.0f);
		lights[1].position = glm::vec3(10.0f, 10.0f, 10.0f);
		lights[2].position = glm::vec3(-10.0f, 10.0f, 10.0f);
		lights[3].position = glm::vec3(10.0f, -10.0f, 10.0f);

		for (unsigned int x = 0; x < row; x++)
		{
			for (unsigned int y = 0; y < col; y++)
			{
				std::shared_ptr<Model> sphere = Model::clone("sphere_" + std::to_string(x * col + y), sphereTemplate.get());

				glm::vec3 pos = glm::vec3(float(x - (col / 2.0f)) * 2.5f, float(y - (row / 2.0f)) * 2.5f, 0.0f);
				sphere->setPosition(pos);
				sphere->setScale(glm::vec3(0.04f));
				sphere->setUniform("albedo", glm::vec3(0.7f, 0.0f, 0.0f));
				sphere->setUniform("roughness", glm::clamp((float)x / (float)(row - 1), 0.05f, 1.0f));
				sphere->setUniform("metallic", glm::clamp((float)y / (float)(col - 1), 0.1f, 1.0f));
				sphere->setUniform("ao", 1.0f);
				sphere->setUniform("exposure", 1.0f);

				for (std::size_t i = 0; i < lights.size(); i++)
				{
					lights[i].color = glm::vec3(300.0f, 300.0f, 300.0f);
					sphere->setUniform("lights[" + std::to_string(i) + "].position", lights[i].position);
					sphere->setUniform("lights[" + std::to_string(i) + "].color", lights[i].color);
				}
				spheres.push_back(sphere);
			}
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
	}

	virtual void render(float deltaTime) override
	{
		for (std::size_t i = 0; i < spheres.size(); i++)
		{
			spheres[i]->setUniform("viewPos", mMainCamera->getPosition());
			spheres[i]->render();
		}

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