#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

#define IRRADIANCE_CUBEMAP_SIZE 128
#define IRRADIANCE_WORK_GROUP_SIZE 8
#define SH_INTERMEDIATE_SIZE (IRRADIANCE_CUBEMAP_SIZE / IRRADIANCE_WORK_GROUP_SIZE)

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> cube;
	std::shared_ptr<Model> cutter;

	std::unique_ptr<Framebuffer> captureFBO;
	std::unique_ptr<Renderbuffer> captureRBO;

	std::shared_ptr<Texture2D> hdrEnvironmentTexture;
	std::shared_ptr<TextureCube> envCubemap;

	std::shared_ptr<Texture2DArray> shIntermediate;
	std::shared_ptr<Texture2D> sh;

	glm::mat4 captureProj;
	std::array<glm::mat4, 6> captureViews;

	struct Light
	{
		glm::vec3 color;
		glm::vec3 position;
	};
	std::array<Light, 6> lights;

	const float exposure = 3.0f;

	Example()
	{
		title = "spherical harmonics lighting";
		settings.vsync = true;
		defaultClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/32.spherical_harmonics_lighting/";
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
		mMainCamera->setPosition(glm::vec3(5.0f, 0.0f, 35.0f));
		mMainCamera->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));

		captureFBO = Framebuffer::create();
		captureRBO = Renderbuffer::create(GL_DEPTH24_STENCIL8, 512, 512);
		captureFBO->addAttachmentRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, captureRBO->getTarget(), captureRBO->getID());

		hdrEnvironmentTexture = Texture2D::createFromFile(texturesDirectory + "/sIBL/Topanga_Forest.hdr", 1, false);

		envCubemap = TextureCube::createFromData("env_cubemap", 512, 512, 1, GL_RGB16F, GL_RGB, GL_FLOAT, nullptr);
		envCubemap->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		envCubemap->setMagFilter(GL_LINEAR);
		envCubemap->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		shIntermediate = Texture2DArray::createFromData(SH_INTERMEDIATE_SIZE * 9, SH_INTERMEDIATE_SIZE, 6, 1, 1, GL_RGBA32F, GL_RGBA, GL_FLOAT, true);
		shIntermediate->setMinFilter(GL_NEAREST);
		shIntermediate->setMagFilter(GL_NEAREST);

		sh = Texture2D::createFromData(9, 1, 1, 1, GL_RGBA32F, GL_RGBA, GL_FLOAT, true);
		sh->setMinFilter(GL_NEAREST);
		sh->setMagFilter(GL_NEAREST);

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

		std::shared_ptr<Program> shProjectionProgram = Program::createFromFiles("sh_projection_program",
			{
				shadersDirectory + "sh_projection.comp"
			}
		);

		shProjectionProgram->apply();
		shProjectionProgram->setUniform("width", envCubemap->getWidth() / 4.0f);
		shProjectionProgram->setUniform("height", envCubemap->getHeight() / 4.0f);
		shProjectionProgram->setUniform("envCubemap", 0);
		envCubemap->bind(0);
		shIntermediate->bindImage(0, 0, 0, GL_WRITE_ONLY, GL_RGBA32F);

		glDispatchCompute(IRRADIANCE_CUBEMAP_SIZE / IRRADIANCE_WORK_GROUP_SIZE, IRRADIANCE_CUBEMAP_SIZE / IRRADIANCE_WORK_GROUP_SIZE, 6);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		std::shared_ptr<Program> shAddProgram = Program::createFromFiles("sh_add_program",
			{
				shadersDirectory + "sh_add.comp"
			}
		);

		shAddProgram->apply();
		shAddProgram->setUniform("shIntermediate", 0);
		shIntermediate->bind(0);
		sh->bindImage(0, 0, 0, GL_WRITE_ONLY, GL_RGBA32F);

		glDispatchCompute(9, 1, 1);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		glViewport(0, 0, mWindowWidth, mWindowHeight);
		
		std::shared_ptr<Material> pbrMat = Material::createFromFiles("pbr_mat",
			{
				shadersDirectory + "pbr.vert",
				shadersDirectory + "pbr.frag"
			},
			{
				{ "albedoMap", modelsDirectory + "/cutter/albedo.png" },
				{ "metallicMap", modelsDirectory + "/cutter/metallic.png" },
				{ "normalMap", modelsDirectory + "/cutter/normal.png" },
				{ "roughnessMap", modelsDirectory + "/cutter/roughness.png" },
				{ "aoMap", modelsDirectory + "/cutter/ao.png" }
			}
		);

		cutter = Model::createFromFile("cutter", modelsDirectory + "/cutter/cutter.fbx", {}, false);
		cutter->setRotation(glm::vec3(0.0f, -90.0f, 0.0f));
		cutter->setScale(glm::vec3(0.05f));

		cutter->setMaterial(pbrMat);
		cutter->setTexture("irradianceSH", sh);

		lights[0].position = glm::vec3(-10.0f, -10.0f, 0.0f);
		lights[1].position = glm::vec3(-10.0f, 10.0f, 0.0f);
		lights[2].position = glm::vec3(10.0f, 10.0f, 0.0f);
		lights[3].position = glm::vec3(10.0f, -10.0f, 0.0f);
		lights[4].position = glm::vec3(0.0f, 0.0f, 10.0f);
		lights[5].position = glm::vec3(0.0f, 0.0f, -10.0f);

		for (std::size_t i = 0; i < lights.size(); i++)
		{
			lights[i].color = glm::vec3(100.0f, 100.0f, 100.0f);
			cutter->setUniform("lights[" + std::to_string(i) + "].position", lights[i].position);
			cutter->setUniform("lights[" + std::to_string(i) + "].color", lights[i].color);
		}
		cutter->setUniform("exposure", exposure);

		std::shared_ptr<Material> skyboxMat = Material::createFromData("skybox_mat",
			{
				shadersDirectory + "skybox.vert",
				shadersDirectory + "skybox.frag"
			},
			{
				{ "environmentMap", envCubemap }
			}
		);
		cube->setMaterial(skyboxMat);
	}

	virtual void render(float deltaTime) override
	{

		glEnable(GL_CULL_FACE);
		cutter->setUniform("viewPos", mMainCamera->getPosition());
		cutter->render();

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