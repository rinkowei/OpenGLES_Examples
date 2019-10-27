#include <examplebase.h>
#include <model.h>
#include <material.h>
#include <buffer.h>
#include <random>
#include <ctime>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::array<std::shared_ptr<Model>, 16> spheres;
	std::shared_ptr<Mesh> blurQuad;
	std::unique_ptr<Framebuffer> hdrFBO;

	Example()
	{
		title = "bloom with hdr";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/26.bloom_with_hdr/";
	}

	~Example()
	{
		spheres.swap(std::array<std::shared_ptr<Model>, 16>());
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		mMainCamera->setPosition(glm::vec3(0.0f, 0.0f, 10.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		// enable cull face
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);

		hdrFBO = Framebuffer::create();

		std::shared_ptr<Texture2D> fragColorTexture = Texture2D::createFromData(defaultWindowWidth, defaultWindowHeight, 1, 1, 1, GL_RGB16F, GL_RGB, GL_FLOAT);
		fragColorTexture->setMinFilter(GL_LINEAR);
		fragColorTexture->setMagFilter(GL_LINEAR);
		fragColorTexture->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		std::shared_ptr<Texture2D> brightColorTexture = Texture2D::createFromData(defaultWindowWidth, defaultWindowHeight, 1, 1, 1, GL_RGB16F, GL_RGB, GL_FLOAT);
		brightColorTexture->setMinFilter(GL_LINEAR);
		brightColorTexture->setMagFilter(GL_LINEAR);
		brightColorTexture->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		hdrFBO->attachRenderTarget(0, fragColorTexture.get(), 0, 0);
		hdrFBO->attachRenderTarget(1, brightColorTexture.get(), 0, 0);

		std::vector<float> vertexAttribs = {
			// positions       // texture coordinates
			1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		   -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		   -1.0f, 1.0f, 0.0f,  0.0f, 1.0f
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

		std::shared_ptr<Material> blurMat = Material::createFromData("blur_mat",
			{
				shadersDirectory + "blur.vert",
				shadersDirectory + "blur.frag"
			},
			{
				{ "image", brightColorTexture }
			}
		);

		blurQuad = Mesh::createWithData("blur_quad", vertices, indices);
		blurQuad->setDrawType(Mesh::DrawType::ELEMENTS);
		blurQuad->setMaterial(blurMat);
		blurQuad->setUniform("blurScale", 2.0f);
		blurQuad->setUniform("blurStrength", 1.5f);
		blurQuad->setUniform("horizontal", 1);

		std::default_random_engine e(time(0));
		std::uniform_real_distribution<double> u(0.5f, 2.0f);

		std::shared_ptr<Model> sphereTemplate = Model::createFromFile("sphere_template", modelsDirectory + "/sphere/sphere.obj",
			{
				shadersDirectory + "color_pass.vert",
				shadersDirectory + "color_pass.frag"
			},
			true
		);
		
		for (size_t i = 0; i < spheres.size(); i++)
		{
			std::shared_ptr<Model> sphere = Model::clone("sphere_" + std::to_string(i), sphereTemplate.get());

			glm::vec3 randomColor = glm::vec3(u(e), u(e), u(e));
			sphere->setUniform("randomColor", randomColor);
			
			glm::vec3 pos = glm::vec3(sin(glm::radians(i * (360.0f / spheres.size()))), cos(glm::radians(i * (360.0f / spheres.size()))), 0.0f) * 3.5f;
			sphere->setPosition(pos);
			sphere->setScale(glm::vec3(0.02f));
			spheres[i] = sphere;
		}
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &destWidth, &destHeight);
		glViewport(0, 0, destWidth, destHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		hdrFBO->bind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		for (size_t i = 0; i < spheres.size(); i++)
		{
			spheres[i]->render();
		}
		hdrFBO->unbind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//glDisable(GL_DEPTH_TEST);
		blurQuad->render();
		//glEnable(GL_DEPTH_TEST);
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